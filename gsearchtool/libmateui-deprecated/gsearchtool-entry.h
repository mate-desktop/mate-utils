/* Copyright (C) 2009 Dennis Cranston
 *
 * This file was modified from gedit (gedit-history-entry.h).
 * Copyright (C) 2006 Paolo Borelli
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

#ifndef __GSEARCH_HISTORY_ENTRY_H__
#define __GSEARCH_HISTORY_ENTRY_H__


G_BEGIN_DECLS

#define GSEARCH_TYPE_HISTORY_ENTRY             (gsearch_history_entry_get_type())
#define GSEARCH_HISTORY_ENTRY(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), GSEARCH_TYPE_HISTORY_ENTRY, GsearchHistoryEntry))
#define GSEARCH_HISTORY_ENTRY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),  GSEARCH_TYPE_HISTORY_ENTRY, GsearchHistoryEntryClass))
#define GSEARCH_IS_HISTORY_ENTRY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), GSEARCH_TYPE_HISTORY_ENTRY))
#define GSEARCH_IS_HISTORY_ENTRY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),  GSEARCH_TYPE_HISTORY_ENTRY))
#define GSEARCH_HISTORY_ENTRY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),  GSEARCH_TYPE_HISTORY_ENTRY, GsearchHistoryEntryClass))


typedef struct _GsearchHistoryEntry        GsearchHistoryEntry;
typedef struct _GsearchHistoryEntryClass   GsearchHistoryEntryClass;
typedef struct _GsearchHistoryEntryPrivate GsearchHistoryEntryPrivate;

struct _GsearchHistoryEntryClass {
	GtkComboBoxClass parent_class;
};

struct _GsearchHistoryEntry {
	GtkComboBox parent_instance;
	GsearchHistoryEntryPrivate* priv;
};

GType gsearch_history_entry_get_type(void) G_GNUC_CONST;

GtkWidget* gsearch_history_entry_new(const gchar* history_id, gboolean enable_completion);
void gsearch_history_entry_prepend_text(GsearchHistoryEntry* entry, const gchar* text);
void gsearch_history_entry_append_text(GsearchHistoryEntry* entry, const gchar* text);
void gsearch_history_entry_clear(GsearchHistoryEntry* entry);

void gsearch_history_entry_set_history_length(GsearchHistoryEntry* entry, guint max_saved);
guint gsearch_history_entry_get_history_length(GsearchHistoryEntry* gentry);

gchar* gsearch_history_entry_get_history_id(GsearchHistoryEntry* entry);

void gsearch_history_entry_set_enable_completion(GsearchHistoryEntry* entry, gboolean enable);
gboolean gsearch_history_entry_get_enable_completion(GsearchHistoryEntry* entry);

GtkWidget* gsearch_history_entry_get_entry(GsearchHistoryEntry* entry);

typedef gchar* (*GsearchHistoryEntryEscapeFunc)(const gchar* str);

void gsearch_history_entry_set_escape_func(GsearchHistoryEntry* entry, GsearchHistoryEntryEscapeFunc escape_func);

G_END_DECLS

#endif /* __GSEARCH_HISTORY_ENTRY_H__ */
