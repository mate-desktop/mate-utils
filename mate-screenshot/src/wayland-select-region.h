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

#ifndef __WAYLAND_SELECT_REGION_H__
#define __WAYLAND_SELECT_REGION_H__

#include <gdk/gdk.h>

G_BEGIN_DECLS

#if defined(ENABLE_WAYLAND) && defined(GDK_WINDOWING_WAYLAND)

gboolean wayland_select_region (GdkRectangle *region);

/* Whether the compositor offers what the selection overlay needs */
gboolean wayland_select_region_is_supported (void);

#else

static inline gboolean
wayland_select_region (GdkRectangle *region)
{
  return FALSE;
}

static inline gboolean
wayland_select_region_is_supported (void)
{
  return FALSE;
}

#endif

G_END_DECLS

#endif
