/* Copyright (C) 2001-2006 Jonathan Blandford <jrb@alum.mit.edu>
 * Copyright (C) 2008 Cosimo Cecchi <cosimoc@gnome.org>
 * Copyright (C) 2012-2021 MATE Developers
 *
 * This file is part of MATE Utils.
 *
 * MATE Utils is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * MATE Utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MATE Utils.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gdk/gdkwayland.h>

#if defined(ENABLE_WAYLAND) && defined(GDK_WINDOWING_WAYLAND)

#include "wayland-screenshot.h"
#include "screenshot-utils.h"
#include <glib/gi18n.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "ext-image-capture-source-client.h"
#include "ext-image-copy-capture-client.h"
#include "wlr-screencopy-client.h"

#include <wayland-client.h>

typedef struct {
  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_shm *shm;
  struct zwlr_screencopy_manager_v1 *screencopy_manager;
  struct ext_output_image_capture_source_manager_v1 *output_image_capture_source_manager;
  struct ext_image_copy_capture_manager_v1 *image_copy_capture_manager;
} ClientData;

typedef struct {
  ClientData *client_data;
  GdkMonitor *monitor;
  struct zwlr_screencopy_frame_v1 *frame;
  struct wl_buffer *buffer;
  struct wl_shm_pool *pool;
  unsigned char *shm_data;
  int width;
  int height;
  int stride;
  int size;
  enum wl_shm_format format;
  gboolean has_format;
  gboolean capture_done;
  gboolean capture_failed;
  gboolean y_inverted;
  struct ext_image_copy_capture_session_v1 *image_copy_capture_session;
  struct ext_image_copy_capture_frame_v1 *image_copy_capture_frame;
} OutputData;

static gboolean wayland_initialized = FALSE;

/* --- Helpers --- */

static gboolean
is_shm_format_supported (enum wl_shm_format format)
{
  return format == WL_SHM_FORMAT_ARGB8888 ||
         format == WL_SHM_FORMAT_XRGB8888 ||
         format == WL_SHM_FORMAT_ABGR8888 ||
         format == WL_SHM_FORMAT_XBGR8888 ||
         format == WL_SHM_FORMAT_BGR888;
}

static gint
get_bpp_from_format (enum wl_shm_format format)
{
  switch (format)
    {
    case WL_SHM_FORMAT_ARGB8888:
    case WL_SHM_FORMAT_XRGB8888:
    case WL_SHM_FORMAT_ABGR8888:
    case WL_SHM_FORMAT_XBGR8888:
      return 4;
    case WL_SHM_FORMAT_BGR888:
      return 3;
    default:
      return 0;
    }
}

static GdkPixbuf *
convert_buffer_to_pixbuf (OutputData *output)
{
  guint8 *data = output->shm_data;
  enum wl_shm_format format = output->format;
  gboolean has_alpha = TRUE;
  GdkPixbuf *pixbuf;

  if (format == WL_SHM_FORMAT_ARGB8888 || format == WL_SHM_FORMAT_XRGB8888)
    {
      for (int y = 0; y < output->height; y++)
        {
          for (int x = 0; x < output->width; x++)
            {
              gint offset = y * output->stride + x * 4;
              guint32 *px = (guint32 *)(gpointer)(data + offset);
              guint8 blue = *px & 0xFF;
              guint8 green = (*px >> 8) & 0xFF;
              guint8 red = (*px >> 16) & 0xFF;
              guint8 alpha = (*px >> 24) & 0xFF;
              data[offset + 0] = red;
              data[offset + 1] = green;
              data[offset + 2] = blue;
              data[offset + 3] = alpha;
            }
        }
    }
  else if (format == WL_SHM_FORMAT_ABGR8888 || format == WL_SHM_FORMAT_XBGR8888)
    {
      for (int y = 0; y < output->height; y++)
        {
          for (int x = 0; x < output->width; x++)
            {
              gint offset = y * output->stride + x * 4;
              guint32 *px = (guint32 *)(gpointer)(data + offset);
              guint8 red = *px & 0xFF;
              guint8 green = (*px >> 8) & 0xFF;
              guint8 blue = (*px >> 16) & 0xFF;
              guint8 alpha = (*px >> 24) & 0xFF;
              data[offset + 0] = red;
              data[offset + 1] = green;
              data[offset + 2] = blue;
              data[offset + 3] = alpha;
            }
        }
    }
  else if (format == WL_SHM_FORMAT_BGR888)
    {
      has_alpha = FALSE;
      for (int y = 0; y < output->height; y++)
        {
          for (int x = 0; x < output->width; x++)
            {
              gint offset = y * output->stride + x * 3;
              guint8 *px = (guint8 *)(gpointer)(data + offset);
              guint8 blue = px[2];
              guint8 green = px[1];
              guint8 red = px[0];
              data[offset + 0] = red;
              data[offset + 1] = green;
              data[offset + 2] = blue;
            }
        }
    }
  else
    {
      g_warning ("Wayland screenshot: unsupported pixel format: 0x%x", format);
      return NULL;
    }

  pixbuf = gdk_pixbuf_new_from_data (data, GDK_COLORSPACE_RGB, has_alpha, 8,
                                     output->width, output->height,
                                     output->stride, NULL, NULL);
  return pixbuf;
}

/* --- Registry --- */

static void
handle_global (void *data, struct wl_registry *registry, uint32_t name,
               const char *interface, uint32_t version)
{
  ClientData *cd = data;

  if (g_strcmp0 (interface, wl_shm_interface.name) == 0)
    cd->shm = wl_registry_bind (registry, name, &wl_shm_interface, 1);
  else if (g_strcmp0 (interface, zwlr_screencopy_manager_v1_interface.name) == 0)
    cd->screencopy_manager = wl_registry_bind (registry, name,
                                               &zwlr_screencopy_manager_v1_interface, 1);
  else if (g_strcmp0 (interface, ext_output_image_capture_source_manager_v1_interface.name) == 0)
    cd->output_image_capture_source_manager = wl_registry_bind (registry, name,
      &ext_output_image_capture_source_manager_v1_interface, 1);
  else if (g_strcmp0 (interface, ext_image_copy_capture_manager_v1_interface.name) == 0)
    cd->image_copy_capture_manager = wl_registry_bind (registry, name,
      &ext_image_copy_capture_manager_v1_interface, 1);
}

static void
handle_global_remove (void *data, struct wl_registry *reg, uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
  .global = handle_global,
  .global_remove = handle_global_remove,
};

/* --- wlr-screencopy frame listener --- */

static void
handle_frame_buffer (void *data, struct zwlr_screencopy_frame_v1 *frame,
                     uint32_t format, uint32_t width, uint32_t height, uint32_t stride)
{
  OutputData *output = data;
  char template[] = "/tmp/mate-screenshot-buffer-XXXXXX";
  int fd;

  output->format = format;
  output->width = width;
  output->height = height;
  output->stride = stride;
  output->size = stride * height;

  g_info ("Wayland screenshot: wlr frame buffer %dx%d stride=%d format=0x%x size=%d",
          width, height, stride, format, output->size);

  fd = mkstemp (template);
  if (fd == -1)
    {
      g_warning ("Wayland screenshot: failed to create temp file");
      output->capture_failed = TRUE;
      return;
    }
  ftruncate (fd, output->size);
  unlink (template);

  output->shm_data = mmap (NULL, output->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (output->shm_data == MAP_FAILED)
    {
      g_warning ("Wayland screenshot: failed to mmap buffer");
      output->shm_data = NULL;
      close (fd);
      output->capture_failed = TRUE;
      return;
    }

  output->pool = wl_shm_create_pool (output->client_data->shm, fd, output->size);
  output->buffer = wl_shm_pool_create_buffer (output->pool, 0, width, height, stride, format);
  close (fd);
  wl_shm_pool_destroy (output->pool);
  output->pool = NULL;

  zwlr_screencopy_frame_v1_copy (frame, output->buffer);
}

static void
handle_frame_flags (void *data, struct zwlr_screencopy_frame_v1 *frame, uint32_t flags)
{
  OutputData *output = data;
  output->y_inverted = flags & ZWLR_SCREENCOPY_FRAME_V1_FLAGS_Y_INVERT ? TRUE : FALSE;
}

static void
handle_frame_ready (void *data, struct zwlr_screencopy_frame_v1 *frame,
                    uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec)
{
  OutputData *output = data;
  output->capture_done = TRUE;
}

static void
handle_frame_failed (void *data, struct zwlr_screencopy_frame_v1 *frame)
{
  OutputData *output = data;
  g_warning ("Wayland screenshot: wlr frame failed");
  output->capture_failed = TRUE;
}

static const struct zwlr_screencopy_frame_v1_listener frame_listener = {
  .buffer = handle_frame_buffer,
  .flags = handle_frame_flags,
  .ready = handle_frame_ready,
  .failed = handle_frame_failed,
};

/* --- ext-image-copy-capture frame listener --- */

static void
handle_ext_frame_transform (void *data, struct ext_image_copy_capture_frame_v1 *frame,
                             uint32_t transform) { }

static void
handle_ext_frame_damage (void *data, struct ext_image_copy_capture_frame_v1 *frame,
                          int32_t x, int32_t y, int32_t width, int32_t height) { }

static void
handle_ext_frame_presentation_time (void *data, struct ext_image_copy_capture_frame_v1 *frame,
                                     uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec) { }

static void
handle_ext_frame_ready (void *data, struct ext_image_copy_capture_frame_v1 *frame)
{
  OutputData *output = data;
  output->capture_done = TRUE;
}

static void
handle_ext_frame_failed (void *data, struct ext_image_copy_capture_frame_v1 *frame,
                          uint32_t reason)
{
  OutputData *output = data;
  g_warning ("Wayland screenshot: ext frame failed (reason %u)", reason);
  output->capture_failed = TRUE;
}

static const struct ext_image_copy_capture_frame_v1_listener ext_frame_listener = {
  .transform = handle_ext_frame_transform,
  .damage = handle_ext_frame_damage,
  .presentation_time = handle_ext_frame_presentation_time,
  .ready = handle_ext_frame_ready,
  .failed = handle_ext_frame_failed,
};

/* --- ext-image-copy-capture session listener --- */

static void
handle_ext_session_buffer_size (void *data, struct ext_image_copy_capture_session_v1 *session,
                                 uint32_t width, uint32_t height)
{
  OutputData *output = data;
  output->width = width;
  output->height = height;
}

static void
handle_ext_session_shm_format (void *data, struct ext_image_copy_capture_session_v1 *session,
                                uint32_t format)
{
  OutputData *output = data;
  if (output->has_format || !is_shm_format_supported (format))
    return;
  output->format = format;
  output->has_format = TRUE;
}

static void
handle_ext_session_dmabuf_device (void *data, struct ext_image_copy_capture_session_v1 *session,
                                   struct wl_array *device)
{
  /* We use SHM, ignore dmabuf */
}

static void
handle_ext_session_dmabuf_format (void *data, struct ext_image_copy_capture_session_v1 *session,
                                   uint32_t format, struct wl_array *modifiers)
{
  /* We use SHM, ignore dmabuf */
}

static void
handle_ext_session_done (void *data, struct ext_image_copy_capture_session_v1 *session)
{
  OutputData *output = data;
  char template[] = "/tmp/mate-screenshot-buffer-XXXXXX";
  int fd;

  if (output->image_copy_capture_frame != NULL)
    return;

  output->stride = output->width * get_bpp_from_format (output->format);
  output->size = output->stride * output->height;

  if (output->size <= 0 || !output->has_format)
    {
      g_warning ("Wayland screenshot: ext session done but invalid size=%d or no format", output->size);
      output->capture_failed = TRUE;
      return;
    }

  fd = mkstemp (template);
  if (fd == -1)
    {
      g_warning ("Wayland screenshot: failed to create temp file");
      output->capture_failed = TRUE;
      return;
    }
  ftruncate (fd, output->size);
  unlink (template);

  output->shm_data = mmap (NULL, output->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (output->shm_data == MAP_FAILED)
    {
      g_warning ("Wayland screenshot: failed to mmap buffer");
      output->shm_data = NULL;
      close (fd);
      output->capture_failed = TRUE;
      return;
    }

  output->pool = wl_shm_create_pool (output->client_data->shm, fd, output->size);
  output->buffer = wl_shm_pool_create_buffer (output->pool, 0,
                                              output->width, output->height,
                                              output->stride, output->format);
  close (fd);
  wl_shm_pool_destroy (output->pool);
  output->pool = NULL;

  if (output->buffer == NULL)
    {
      g_warning ("Wayland screenshot: failed to create wl_buffer");
      output->capture_failed = TRUE;
      return;
    }

  output->image_copy_capture_frame =
    ext_image_copy_capture_session_v1_create_frame (session);
  ext_image_copy_capture_frame_v1_add_listener (output->image_copy_capture_frame,
                                                 &ext_frame_listener, output);
  ext_image_copy_capture_frame_v1_attach_buffer (output->image_copy_capture_frame,
                                                  output->buffer);
  ext_image_copy_capture_frame_v1_damage_buffer (output->image_copy_capture_frame,
                                                  0, 0, INT32_MAX, INT32_MAX);
  ext_image_copy_capture_frame_v1_capture (output->image_copy_capture_frame);
}

static void
handle_ext_session_stopped (void *data, struct ext_image_copy_capture_session_v1 *session)
{
  g_warning ("Wayland screenshot: ext session stopped");
}

static const struct ext_image_copy_capture_session_v1_listener ext_session_listener = {
  .buffer_size = handle_ext_session_buffer_size,
  .shm_format = handle_ext_session_shm_format,
  .dmabuf_device = handle_ext_session_dmabuf_device,
  .dmabuf_format = handle_ext_session_dmabuf_format,
  .done = handle_ext_session_done,
  .stopped = handle_ext_session_stopped,
};

/* --- Output data cleanup --- */

static void
free_output_data (gpointer data)
{
  OutputData *output = data;
  if (output->shm_data != NULL)
    munmap (output->shm_data, output->size);
  if (output->buffer != NULL)
    wl_buffer_destroy (output->buffer);
  if (output->frame != NULL)
    zwlr_screencopy_frame_v1_destroy (output->frame);
  if (output->image_copy_capture_session != NULL)
    ext_image_copy_capture_session_v1_destroy (output->image_copy_capture_session);
  if (output->image_copy_capture_frame != NULL)
    ext_image_copy_capture_frame_v1_destroy (output->image_copy_capture_frame);
  g_free (output);
}

static void
free_client_data (ClientData *cd)
{
  if (cd->shm != NULL)
    wl_shm_destroy (cd->shm);
  if (cd->screencopy_manager != NULL)
    zwlr_screencopy_manager_v1_destroy (cd->screencopy_manager);
  if (cd->output_image_capture_source_manager != NULL)
    ext_output_image_capture_source_manager_v1_destroy (cd->output_image_capture_source_manager);
  if (cd->image_copy_capture_manager != NULL)
    ext_image_copy_capture_manager_v1_destroy (cd->image_copy_capture_manager);
  wl_registry_destroy (cd->registry);
}

/* --- Capture --- */

static GdkPixbuf *
compose_screenshot (GList *outputs)
{
  gint total_width = 0;
  gint total_height = 0;

  /* Calculate the virtual framebuffer size using GDK logical coordinates */
  for (GList *elem = outputs; elem; elem = elem->next)
    {
      OutputData *od = elem->data;
      GdkRectangle geometry;

      gdk_monitor_get_geometry (od->monitor, &geometry);

      gint sx = geometry.x + geometry.width;
      gint sy = geometry.y + geometry.height;
      if (sx > total_width)
        total_width = sx;
      if (sy > total_height)
        total_height = sy;
    }

  if (total_width == 0 || total_height == 0)
    return NULL;

  /* Create the destination pixbuf for the virtual framebuffer */
  GdkPixbuf *dest = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                    total_width, total_height);

  /* Composite each output into the destination */
  for (GList *elem = outputs; elem; elem = elem->next)
    {
      OutputData *od = elem->data;
      GdkRectangle geometry;
      GdkPixbuf *raw;
      gdouble scale;
      gdouble composite_scale;

      if (od->capture_failed)
        continue;

      raw = convert_buffer_to_pixbuf (od);
      if (raw == NULL)
        continue;

      /* Handle Y-inverted buffer from wlr-screencopy */
      if (od->y_inverted)
        {
          GdkPixbuf *flipped = gdk_pixbuf_rotate_simple (raw, GDK_PIXBUF_ROTATE_UPSIDEDOWN);
          g_object_unref (raw);
          raw = flipped;
        }

      /* The capture buffer is in physical pixels; the destination uses the
       * same GDK logical coordinates as the rest of the application, so we
       * downscale each output by its own scale factor. */
      gdk_monitor_get_geometry (od->monitor, &geometry);
      scale = (gdouble) od->width / (gdouble) geometry.width;
      if (scale < 1)
        scale = 1;

      composite_scale = 1.0 / scale;

      g_info ("Wayland screenshot: compositing output %dx%d at (%d,%d) scale %.2f",
              od->width, od->height, geometry.x, geometry.y, scale);

      gdk_pixbuf_composite (raw, dest,
                            geometry.x, geometry.y,
                            geometry.width, geometry.height,
                            geometry.x, geometry.y,
                            composite_scale, composite_scale,
                            GDK_INTERP_BILINEAR, 255);
      g_object_unref (raw);
    }

  return dest;
}

static GdkPixbuf *
capture_screenshot (G_GNUC_UNUSED gboolean capture_all_monitors, gboolean show_mouse)
{
  ClientData client_data = { 0 };
  GList *outputs = NULL;
  GdkPixbuf *screenshot = NULL;
  gboolean failure = FALSE;
  int n_monitors;

  client_data.display = gdk_wayland_display_get_wl_display (gdk_display_get_default ());
  client_data.registry = wl_display_get_registry (client_data.display);
  wl_registry_add_listener (client_data.registry, &registry_listener, &client_data);
  wl_display_roundtrip (client_data.display);

  if (client_data.shm == NULL)
    {
      g_warning ("Wayland screenshot: wl_shm not available");
      free_client_data (&client_data);
      return NULL;
    }

  if (client_data.output_image_capture_source_manager == NULL &&
      client_data.image_copy_capture_manager == NULL &&
      client_data.screencopy_manager == NULL)
    {
      g_warning ("Wayland screenshot: no capture protocol available");
      free_client_data (&client_data);
      return NULL;
    }

  n_monitors = gdk_display_get_n_monitors (gdk_display_get_default ());
  for (int i = 0; i < n_monitors; i++)
    {
      OutputData *output;
      GdkMonitor *monitor = gdk_display_get_monitor (gdk_display_get_default (), i);
      struct wl_output *wl_output = gdk_wayland_monitor_get_wl_output (monitor);

      if (wl_output == NULL)
        {
          g_warning ("Wayland screenshot: no wl_output for monitor %d", i);
          continue;
        }

      output = g_new0 (OutputData, 1);
      output->monitor = monitor;
      output->client_data = &client_data;
      outputs = g_list_append (outputs, output);

      if (client_data.image_copy_capture_manager != NULL &&
          client_data.output_image_capture_source_manager != NULL)
        {
          struct ext_image_capture_source_v1 *source =
            ext_output_image_capture_source_manager_v1_create_source (
              client_data.output_image_capture_source_manager, wl_output);

          guint32 options = show_mouse ? EXT_IMAGE_COPY_CAPTURE_MANAGER_V1_OPTIONS_PAINT_CURSORS : 0;
          output->image_copy_capture_session =
            ext_image_copy_capture_manager_v1_create_session (
              client_data.image_copy_capture_manager, source, options);

          ext_image_copy_capture_session_v1_add_listener (
            output->image_copy_capture_session, &ext_session_listener, output);

          ext_image_capture_source_v1_destroy (source);
        }
      else if (client_data.screencopy_manager != NULL)
        {
          output->frame = zwlr_screencopy_manager_v1_capture_output (
            client_data.screencopy_manager, show_mouse ? 1 : 0, wl_output);
          zwlr_screencopy_frame_v1_add_listener (output->frame, &frame_listener, output);
        }
    }

  /* Dispatch events until all outputs are captured or failed */
  while (TRUE)
    {
      gboolean done = TRUE;
      for (GList *elem = outputs; elem; elem = elem->next)
        {
          OutputData *od = elem->data;
          if (!od->capture_done && !od->capture_failed)
            done = FALSE;
          if (od->capture_failed)
            failure = TRUE;
        }
      if (done)
        break;
      wl_display_dispatch (client_data.display);
    }

  if (!failure && outputs != NULL)
    {
      screenshot = compose_screenshot (outputs);
    }

  free_client_data (&client_data);
  g_list_free_full (outputs, free_output_data);

  return screenshot;
}

/* --- Public API --- */

gboolean
wayland_screenshot_init (void)
{
  GdkDisplay *gdk_display;

  gdk_display = gdk_display_get_default ();
  if (!GDK_IS_WAYLAND_DISPLAY (gdk_display))
    return FALSE;

  wayland_initialized = TRUE;

  g_info ("Wayland screenshot: support initialized");

  return TRUE;
}

void
wayland_screenshot_cleanup (void)
{
  wayland_initialized = FALSE;
}

gboolean
wayland_screenshot_is_available (void)
{
  return wayland_initialized;
}

GdkPixbuf *
wayland_screenshot_capture_screen (gboolean include_pointer)
{
  if (!wayland_initialized)
    return NULL;

  return capture_screenshot (TRUE, include_pointer);
}

GdkPixbuf *
wayland_screenshot_capture_output (GdkMonitor *monitor, gboolean include_pointer)
{
  if (!wayland_initialized || !monitor)
    return NULL;

  return capture_screenshot (FALSE, include_pointer);
}

GdkPixbuf *
wayland_screenshot_capture_region (GdkRectangle *region, gboolean include_pointer)
{
  GdkDisplay *display;
  GdkMonitor *monitor;

  if (!wayland_initialized || !region)
    return NULL;

  display = gdk_display_get_default ();
  monitor = gdk_display_get_monitor_at_point (display, region->x, region->y);

  if (!monitor)
    monitor = gdk_display_get_primary_monitor (display);
  if (!monitor)
    monitor = gdk_display_get_monitor (display, 0);

  return wayland_screenshot_capture_output (monitor, include_pointer);
}

#endif /* ENABLE_WAYLAND && GDK_WINDOWING_WAYLAND */
