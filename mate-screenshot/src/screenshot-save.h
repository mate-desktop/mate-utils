/* Copyright (C) 2001-2006 Jonathan Blandford <jrb@alum.mit.edu>
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

#ifndef __SCREENSHOT_SAVE_H__
#define __SCREENSHOT_SAVE_H__

#include <gtk/gtk.h>

typedef void (*SaveFunction) (gpointer data);

void        screenshot_save_start        (GdkPixbuf    *pixbuf,
					  SaveFunction  callback,
					  gpointer      user_data);
const char *screenshot_save_get_filename (void);


#endif /* __SCREENSHOT_SAVE_H__ */
