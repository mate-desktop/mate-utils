/* Copyright (C) 2006  Emmanuele Bassi <ebassi@gmail.com>
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

#ifndef __GDICT_DEBUG_H__
#define __GDICT_DEBUG_H__

#include <glib.h>

G_BEGIN_DECLS

typedef enum {
  GDICT_DEBUG_MISC    = 1 << 0,
  GDICT_DEBUG_CONTEXT = 1 << 1,
  GDICT_DEBUG_DICT    = 1 << 2,
  GDICT_DEBUG_SOURCE  = 1 << 3,
  GDICT_DEBUG_LOADER  = 1 << 4,
  GDICT_DEBUG_CHOOSER = 1 << 5,
  GDICT_DEBUG_DEFBOX  = 1 << 6,
  GDICT_DEBUG_SPELLER = 1 << 7
} GdictDebugFlags;

#ifdef GDICT_ENABLE_DEBUG

#define GDICT_NOTE(type,x,a...)                 G_STMT_START {  \
        if (gdict_debug_flags & GDICT_DEBUG_##type) {           \
          g_message ("[" #type "]: " G_STRLOC ": " x, ##a);     \
        }                                       } G_STMT_END

#else

#define GDICT_NOTE(type,x,a...)

#endif /* !GDICT_ENABLE_DEBUG */

extern guint gdict_debug_flags;

G_END_DECLS

#endif /* __GDICT_DEBUG_H__ */
