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

#ifndef __WAYLAND_SCREENSHOT_H__
#define __WAYLAND_SCREENSHOT_H__

#include <gdk/gdk.h>

G_BEGIN_DECLS

#if defined(ENABLE_WAYLAND) && defined(GDK_WINDOWING_WAYLAND)

typedef struct _WaylandScreenshot WaylandScreenshot;

/* Initialize Wayland screenshot support */
gboolean wayland_screenshot_init (void);

/* Cleanup Wayland screenshot support */
void wayland_screenshot_cleanup (void);

/* Check if we're running on Wayland */
gboolean wayland_screenshot_is_available (void);

/* Capture the entire screen */
GdkPixbuf *wayland_screenshot_capture_screen (gboolean include_pointer);

/* Capture a specific output */
GdkPixbuf *wayland_screenshot_capture_output (GdkMonitor *monitor, gboolean include_pointer);

/* Capture a specific region */
GdkPixbuf *wayland_screenshot_capture_region (GdkRectangle *region, gboolean include_pointer);

#else /* !(ENABLE_WAYLAND && GDK_WINDOWING_WAYLAND) */

/* Stub implementations when Wayland support is disabled */
static inline gboolean
wayland_screenshot_init (void)
{
  return FALSE;
}

static inline void
wayland_screenshot_cleanup (void)
{
}

static inline gboolean
wayland_screenshot_is_available (void)
{
  return FALSE;
}

static inline GdkPixbuf *
wayland_screenshot_capture_screen (gboolean include_pointer)
{
  return NULL;
}

static inline GdkPixbuf *
wayland_screenshot_capture_output (GdkMonitor *monitor, gboolean include_pointer)
{
  return NULL;
}

static inline GdkPixbuf *
wayland_screenshot_capture_region (GdkRectangle *region, gboolean include_pointer)
{
  return NULL;
}

#endif /* ENABLE_WAYLAND && GDK_WINDOWING_WAYLAND */

G_END_DECLS

#endif /* __WAYLAND_SCREENSHOT_H__ */