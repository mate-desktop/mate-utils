/* Copyright (C) 2001-2006 Jonathan Blandford <jrb@alum.mit.edu>
 * Copyright (C) 2008 Cosimo Cecchi <cosimoc@gnome.org>
 * Copyright (C) 2012-2026 MATE Developers
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

#include "wayland-select-region.h"

#include <gtk-layer-shell.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <string.h>

#define BACKGROUND_TRANSPARENCY 0.4
#define DROP_SHADOW_SIZE 2
#define SIZE_DISPLAY_OFFSET 4

enum {
  ANCHOR_UNSET = 0,
  ANCHOR_NONE = 1,
  ANCHOR_TOP = 2,
  ANCHOR_LEFT = 4
};

typedef struct {
  gboolean left_pressed;
  gboolean rubber_banding;
  gboolean cancelled;
  gboolean move_rectangle;
  gboolean finished;
  gint anchor;
  gint x;
  gint y;
  gint x_root;
  gint y_root;
  gint cursor_x;
  gint cursor_y;
  cairo_rectangle_int_t old_text_rect;
  cairo_rectangle_int_t rectangle;
  PangoLayout *text_layout;
  GSList *overlays;
} RubberBandData;

typedef struct {
  GtkWidget *window;
  GdkMonitor *monitor;
  GdkRectangle monitor_geometry;
} OverlayData;

static void
destroy_overlay (OverlayData *overlay)
{
  gtk_widget_destroy (overlay->window);
  g_free (overlay);
}

static void
close_overlays (RubberBandData *rbdata)
{
  GSList *l;
  OverlayData *first_overlay;
  GdkFrameClock *frame_clock;

  rbdata->finished = TRUE;

  for (l = rbdata->overlays; l != NULL; l = l->next)
    {
      OverlayData *data = l->data;
      gtk_widget_queue_draw (data->window);
    }

  if (rbdata->overlays != NULL)
    {
      first_overlay = rbdata->overlays->data;
      frame_clock = gtk_widget_get_frame_clock (first_overlay->window);
      g_signal_connect (frame_clock, "after-paint", G_CALLBACK (gtk_main_quit), NULL);
    }
  else
    {
      gtk_main_quit ();
    }
}

static gboolean
cb_key_pressed (GtkWidget      *widget,
                GdkEventKey    *event,
                RubberBandData *rbdata)
{
  guint key = event->keyval;

  if (key == GDK_KEY_Escape)
    {
      rbdata->cancelled = TRUE;
      close_overlays (rbdata);
      return TRUE;
    }

  if (rbdata->left_pressed && (key == GDK_KEY_Control_L || key == GDK_KEY_Control_R))
    {
      rbdata->move_rectangle = TRUE;
      return TRUE;
    }

  return FALSE;
}

static gboolean
cb_key_released (GtkWidget      *widget,
                 GdkEventKey    *event,
                 RubberBandData *rbdata)
{
  guint key = event->keyval;

  if (rbdata->left_pressed && (key == GDK_KEY_Control_L || key == GDK_KEY_Control_R))
    {
      rbdata->move_rectangle = FALSE;
      rbdata->anchor = ANCHOR_UNSET;
      return TRUE;
    }

  return FALSE;
}

static gboolean
cb_draw (GtkWidget      *widget,
         cairo_t        *cr,
         RubberBandData *rbdata)
{
  cairo_set_source_rgba (cr, 0, 0, 0, rbdata->finished ? 0 : BACKGROUND_TRANSPARENCY);
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);

  if (rbdata->rubber_banding)
    {
      OverlayData *overlay;
      cairo_rectangle_int_t intersection;
      cairo_rectangle_int_t selection_rect_local = rbdata->rectangle;

      overlay = g_object_get_data (G_OBJECT (widget), "overlay");

      selection_rect_local.x -= overlay->monitor_geometry.x;
      selection_rect_local.y -= overlay->monitor_geometry.y;

      if (gdk_rectangle_intersect ((GdkRectangle *) &selection_rect_local,
                                   &(GdkRectangle) {0, 0, overlay->monitor_geometry.width, overlay->monitor_geometry.height},
                                   &intersection))
        {
          gchar *coords;
          gint rect_width, rect_height, x_offset, y_offset, text_width, text_height, x_text, y_text;

          cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
          gdk_cairo_rectangle (cr, &intersection);
          cairo_fill (cr);

          cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
          cairo_set_source_rgba (cr, 1.0f, 1.0f, 1.0f, 0.1f);
          gdk_cairo_rectangle (cr, &intersection);
          cairo_fill (cr);

          if (rbdata->cursor_x < overlay->monitor_geometry.x ||
              rbdata->cursor_x > overlay->monitor_geometry.x + overlay->monitor_geometry.width ||
              rbdata->cursor_y < overlay->monitor_geometry.y ||
              rbdata->cursor_y > overlay->monitor_geometry.y + overlay->monitor_geometry.height)
            return FALSE;

          rect_width = rbdata->rectangle.width;
          rect_height = rbdata->rectangle.height;

          if (rbdata->rectangle.x < 0) rect_width += rbdata->rectangle.x;
          if (rbdata->rectangle.y < 0) rect_height += rbdata->rectangle.y;

          if (rbdata->text_layout == NULL)
            {
              rbdata->text_layout = pango_cairo_create_layout (cr);
              pango_layout_set_font_description (rbdata->text_layout,
                pango_font_description_from_string ("monospace 10"));
            }

          coords = g_strdup_printf ("%d x %d", rect_width, rect_height);
          pango_layout_set_text (rbdata->text_layout, coords, -1);
          pango_layout_get_pixel_size (rbdata->text_layout, &text_width, &text_height);
          g_free (coords);

          x_offset = SIZE_DISPLAY_OFFSET;
          y_offset = SIZE_DISPLAY_OFFSET;

          if (rbdata->cursor_x - overlay->monitor_geometry.x > overlay->monitor_geometry.width - text_width - x_offset)
            x_offset -= text_width + SIZE_DISPLAY_OFFSET;

          if (rbdata->cursor_y - overlay->monitor_geometry.y > overlay->monitor_geometry.height - text_height - y_offset)
            y_offset -= text_height + SIZE_DISPLAY_OFFSET;

          rbdata->old_text_rect.x = rbdata->cursor_x + x_offset;
          rbdata->old_text_rect.y = rbdata->cursor_y + y_offset;
          rbdata->old_text_rect.width = text_width + DROP_SHADOW_SIZE;
          rbdata->old_text_rect.height = text_height + DROP_SHADOW_SIZE;

          x_text = rbdata->old_text_rect.x - overlay->monitor_geometry.x;
          y_text = rbdata->old_text_rect.y - overlay->monitor_geometry.y;

          cairo_move_to (cr, x_text + DROP_SHADOW_SIZE, y_text + DROP_SHADOW_SIZE);
          cairo_set_source_rgb (cr, 0, 0, 0);
          pango_cairo_show_layout (cr, rbdata->text_layout);

          cairo_move_to (cr, x_text, y_text);
          cairo_set_source_rgb (cr, 1, 1, 1);
          pango_cairo_show_layout (cr, rbdata->text_layout);
        }
    }

  return FALSE;
}

static gboolean
cb_button_pressed (GtkWidget      *widget,
                   GdkEventButton *event,
                   RubberBandData *rbdata)
{
  if (event->button == 1)
    {
      OverlayData *overlay;

      overlay = g_object_get_data (G_OBJECT (widget), "overlay");

      rbdata->left_pressed = TRUE;
      rbdata->x_root = overlay->monitor_geometry.x + event->x;
      rbdata->y_root = overlay->monitor_geometry.y + event->y;

      return TRUE;
    }

  return FALSE;
}

static gboolean
cb_button_released (GtkWidget      *widget,
                    GdkEventButton *event,
                    RubberBandData *rbdata)
{
  if (event->button == 1)
    {
      if (rbdata->rubber_banding && rbdata->rectangle.width > 0 && rbdata->rectangle.height > 0)
        {
          rbdata->left_pressed = rbdata->rubber_banding = FALSE;
          close_overlays (rbdata);
          return TRUE;
        }
      rbdata->left_pressed = rbdata->rubber_banding = FALSE;
    }

  return FALSE;
}

static gboolean
cb_motion_notify (GtkWidget      *widget,
                  GdkEventMotion *event,
                  RubberBandData *rbdata)
{
  if (rbdata->left_pressed)
    {
      cairo_rectangle_int_t *new_rect;
      cairo_rectangle_int_t old_rect, intersect;
      cairo_region_t *region;
      OverlayData *overlay;
      gint event_x_root, event_y_root;
      GSList *l;

      overlay = g_object_get_data (G_OBJECT (widget), "overlay");
      rbdata->cursor_x = event_x_root = overlay->monitor_geometry.x + event->x - 1;
      rbdata->cursor_y = event_y_root = overlay->monitor_geometry.y + event->y - 1;
      new_rect = &rbdata->rectangle;

      if (!rbdata->rubber_banding)
        {
          rbdata->rubber_banding = TRUE;
          rbdata->x = event_x_root;
          rbdata->y = event_y_root;
          old_rect.x = rbdata->x;
          old_rect.y = rbdata->y;
          old_rect.height = old_rect.width = 1;
        }
      else
        {
          old_rect.x = new_rect->x;
          old_rect.y = new_rect->y;
          old_rect.width = new_rect->width;
          old_rect.height = new_rect->height;
        }

      if (rbdata->move_rectangle)
        {
          if (rbdata->anchor == ANCHOR_UNSET)
            {
              rbdata->anchor = ANCHOR_NONE;
              rbdata->anchor |= (event_x_root < rbdata->x) ? ANCHOR_LEFT : 0;
              rbdata->anchor |= (event_y_root < rbdata->y) ? ANCHOR_TOP : 0;
            }

          if (rbdata->anchor & ANCHOR_LEFT)
            rbdata->x = (new_rect->x = event_x_root) + new_rect->width;
          else
            rbdata->x = new_rect->x = event_x_root - new_rect->width;

          if (rbdata->anchor & ANCHOR_TOP)
            rbdata->y = (new_rect->y = event_y_root) + new_rect->height;
          else
            rbdata->y = new_rect->y = event_y_root - new_rect->height;
        }
      else
        {
          new_rect->x = MIN (rbdata->x, event_x_root);
          new_rect->y = MIN (rbdata->y, event_y_root);
          new_rect->width = ABS (rbdata->x - event_x_root) + 1;
          new_rect->height = ABS (rbdata->y - event_y_root) + 1;
        }

      region = cairo_region_create_rectangle (&old_rect);
      cairo_region_union_rectangle (region, new_rect);

      if (gdk_rectangle_intersect (&old_rect, new_rect, &intersect) && intersect.width > 2 && intersect.height > 2)
        {
          intersect.x += 1;
          intersect.width -= 2;
          intersect.y += 1;
          intersect.height -= 2;
          cairo_region_subtract_rectangle (region, &intersect);
        }

      rbdata->old_text_rect.x -= rbdata->old_text_rect.width;
      rbdata->old_text_rect.y -= rbdata->old_text_rect.height;
      rbdata->old_text_rect.width += 2 * rbdata->old_text_rect.width;
      rbdata->old_text_rect.height += 2 * rbdata->old_text_rect.height;
      cairo_region_union_rectangle (region, &(rbdata->old_text_rect));

      for (l = rbdata->overlays; l != NULL; l = l->next)
        {
          OverlayData *data = l->data;
          cairo_region_translate (region, -(data->monitor_geometry.x), -(data->monitor_geometry.y));
          gdk_window_invalidate_region (gtk_widget_get_window (data->window), region, TRUE);
          cairo_region_translate (region, data->monitor_geometry.x, data->monitor_geometry.y);
        }

      cairo_region_destroy (region);

      return TRUE;
    }

  return FALSE;
}

gboolean
wayland_select_region (GdkRectangle *region)
{
  RubberBandData rbdata = {0};
  GdkCursor *xhair_cursor;
  GdkDisplay *display;
  int n_monitors;
  GdkRectangle root_geometry = {0};

  if (!gtk_layer_is_supported ())
    {
      g_warning ("Compositor does not support zwlr_layer_shell_v1");
      return FALSE;
    }

  display = gdk_display_get_default ();
  xhair_cursor = gdk_cursor_new_for_display (display, GDK_CROSSHAIR);
  n_monitors = gdk_display_get_n_monitors (display);

  for (int n_monitor = 0; n_monitor < n_monitors; n_monitor++)
    {
      GdkMonitor *monitor;
      GtkWidget *window;
      OverlayData *overlay;

      monitor = gdk_display_get_monitor (display, n_monitor);
      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      overlay = g_new0 (OverlayData, 1);
      overlay->window = window;
      overlay->monitor = monitor;
      gdk_monitor_get_geometry (monitor, &(overlay->monitor_geometry));
      gdk_rectangle_union (&(overlay->monitor_geometry), &root_geometry, &root_geometry);
      rbdata.overlays = g_slist_append (rbdata.overlays, overlay);
      g_object_set_data (G_OBJECT (window), "overlay", overlay);

      gtk_window_set_title (GTK_WINDOW (window), _("MATE Screenshot overlay"));
      gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
      gtk_window_set_deletable (GTK_WINDOW (window), FALSE);
      gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
      gtk_widget_set_can_focus (window, FALSE);
      gtk_widget_set_app_paintable (window, TRUE);
      gtk_widget_add_events (window,
                             GDK_BUTTON_RELEASE_MASK |
                             GDK_BUTTON_PRESS_MASK |
                             GDK_POINTER_MOTION_MASK |
                             GDK_KEY_PRESS_MASK |
                             GDK_ENTER_NOTIFY_MASK);
      gtk_widget_set_visual (window, gdk_screen_get_rgba_visual (gdk_screen_get_default ()));

      gtk_layer_init_for_window (GTK_WINDOW (window));
      gtk_layer_set_monitor (GTK_WINDOW (window), monitor);
      gtk_layer_set_layer (GTK_WINDOW (window), GTK_LAYER_SHELL_LAYER_OVERLAY);
      gtk_layer_set_keyboard_mode (GTK_WINDOW (window), GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE);
      gtk_layer_set_anchor (GTK_WINDOW (window), GTK_LAYER_SHELL_EDGE_TOP, TRUE);
      gtk_layer_set_anchor (GTK_WINDOW (window), GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
      gtk_layer_set_anchor (GTK_WINDOW (window), GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
      gtk_layer_set_anchor (GTK_WINDOW (window), GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);

      /* Allow dragging from screen edges */
      gtk_layer_set_margin (GTK_WINDOW (window), GTK_LAYER_SHELL_EDGE_TOP, -1);
      gtk_layer_set_margin (GTK_WINDOW (window), GTK_LAYER_SHELL_EDGE_LEFT, -1);
      gtk_layer_set_margin (GTK_WINDOW (window), GTK_LAYER_SHELL_EDGE_BOTTOM, -1);
      gtk_layer_set_margin (GTK_WINDOW (window), GTK_LAYER_SHELL_EDGE_RIGHT, -1);
      gtk_layer_set_exclusive_zone (GTK_WINDOW (window), -1);

      g_signal_connect (window, "key-press-event", G_CALLBACK (cb_key_pressed), &rbdata);
      g_signal_connect (window, "key-release-event", G_CALLBACK (cb_key_released), &rbdata);
      g_signal_connect (window, "draw", G_CALLBACK (cb_draw), &rbdata);
      g_signal_connect (window, "button-press-event", G_CALLBACK (cb_button_pressed), &rbdata);
      g_signal_connect (window, "button-release-event", G_CALLBACK (cb_button_released), &rbdata);
      g_signal_connect (window, "motion-notify-event", G_CALLBACK (cb_motion_notify), &rbdata);

      gtk_widget_show_all (window);
      gdk_window_set_cursor (gtk_widget_get_window (window), xhair_cursor);
    }

  gtk_main ();

  g_slist_foreach (rbdata.overlays, (GFunc) destroy_overlay, NULL);
  g_slist_free (rbdata.overlays);
  g_object_unref (xhair_cursor);
  if (rbdata.text_layout != NULL)
    g_object_unref (rbdata.text_layout);

  if (rbdata.cancelled)
    return FALSE;

  region->x = rbdata.rectangle.x;
  region->y = rbdata.rectangle.y;
  region->width = rbdata.rectangle.width;
  region->height = rbdata.rectangle.height;

  /* Clamp to screen bounds */
  if (region->x < 0)
    region->width += region->x;
  if (region->y < 0)
    region->height += region->y;

  region->x = MAX (0, MIN (region->x, root_geometry.width));
  region->y = MAX (0, MIN (region->y, root_geometry.height));

  if (region->x + region->width > root_geometry.width)
    region->width = root_geometry.width - region->x;
  if (region->y + region->height > root_geometry.height)
    region->height = root_geometry.height - region->y;

  if (region->width == 0 && region->x == root_geometry.width)
    {
      region->width = 1;
      region->x--;
    }
  if (region->height == 0 && region->y == root_geometry.height)
    {
      region->height = 1;
      region->y--;
    }

  return TRUE;
}

#endif /* ENABLE_WAYLAND && GDK_WINDOWING_WAYLAND */
