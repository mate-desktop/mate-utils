/*
 * gsearchtool-entry.c
 *
 * This file was modified from gedit (gedit-history-entry.c).
 *
 * Copyright (C) 2009 - Dennis Cranston
 * Copyright (C) 2006 - Paolo Borelli
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Paolo Borelli  (gedit-history-entry.c)
 *
 * Gsearchtool port by Dennis Cranston <dennis_cranston@yahoo.com>
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "gsearchtool-entry.h"

enum {
	PROP_0,
	PROP_HISTORY_ID,
	PROP_HISTORY_LENGTH
};

#define MIN_ITEM_LEN 0
#define GSEARCH_HISTORY_ENTRY_HISTORY_LENGTH_DEFAULT 10
#define GSEARCH_HISTORY_ENTRY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), \
                                                  GSEARCH_TYPE_HISTORY_ENTRY, \
                                                  GsearchHistoryEntryPrivate))

struct _GsearchHistoryEntryPrivate
{
	gchar              *history_id;
	guint               history_length;

	GtkEntryCompletion *completion;

	GSettings          *settings;
};

G_DEFINE_TYPE (GsearchHistoryEntry, gsearch_history_entry, GTK_TYPE_COMBO_BOX)

static void
gsearch_history_entry_set_property (GObject      *object,
				    guint         prop_id,
				    const GValue *value,
				    GParamSpec   *spec)
{
	GsearchHistoryEntry *entry;

	g_return_if_fail (GSEARCH_IS_HISTORY_ENTRY (object));

	entry = GSEARCH_HISTORY_ENTRY (object);

	switch (prop_id) {
	case PROP_HISTORY_ID:
		entry->priv->history_id = g_value_dup_string (value);
		break;
	case PROP_HISTORY_LENGTH:
		gsearch_history_entry_set_history_length (entry,
						     g_value_get_uint (value));
		break;
	default:
		break;
	}
}

static void
gsearch_history_entry_get_property (GObject    *object,
				    guint       prop_id,
				    GValue     *value,
				    GParamSpec *spec)
{
	GsearchHistoryEntryPrivate *priv;

	g_return_if_fail (GSEARCH_IS_HISTORY_ENTRY (object));

	priv = GSEARCH_HISTORY_ENTRY (object)->priv;

	switch (prop_id) {
	case PROP_HISTORY_ID:
		g_value_set_string (value, priv->history_id);
		break;
	case PROP_HISTORY_LENGTH:
		g_value_set_uint (value, priv->history_length);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, spec);
	}
}

static void
gsearch_history_entry_destroy (GtkWidget *object)
{
	gsearch_history_entry_set_enable_completion (GSEARCH_HISTORY_ENTRY (object),
						   FALSE);

	GTK_WIDGET_CLASS (gsearch_history_entry_parent_class)->destroy (object);
}

static void
gsearch_history_entry_finalize (GObject *object)
{
	GsearchHistoryEntryPrivate *priv;

	priv = GSEARCH_HISTORY_ENTRY (object)->priv;

	g_free (priv->history_id);

	if (priv->settings != NULL)
	{
		g_object_unref (G_OBJECT (priv->settings));
		priv->settings = NULL;
	}

	G_OBJECT_CLASS (gsearch_history_entry_parent_class)->finalize (object);
}

static void
gsearch_history_entry_class_init (GsearchHistoryEntryClass *klass)
{
	GObjectClass   *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *gtkwidget_class = GTK_WIDGET_CLASS (klass);

	object_class->set_property = gsearch_history_entry_set_property;
	object_class->get_property = gsearch_history_entry_get_property;
	object_class->finalize = gsearch_history_entry_finalize;
	gtkwidget_class->destroy = gsearch_history_entry_destroy;

	g_object_class_install_property (object_class,
					 PROP_HISTORY_ID,
					 g_param_spec_string ("history-id",
							      "History ID",
							      "History ID",
							      NULL,
							      G_PARAM_READWRITE |
							      G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (object_class,
					 PROP_HISTORY_LENGTH,
					 g_param_spec_uint ("history-length",
							    "Max History Length",
							    "Max History Length",
							    0,
							    G_MAXUINT,
							    GSEARCH_HISTORY_ENTRY_HISTORY_LENGTH_DEFAULT,
							    G_PARAM_READWRITE |
							    G_PARAM_STATIC_STRINGS));

	/* TODO: Add enable-completion property */

	g_type_class_add_private (object_class, sizeof(GsearchHistoryEntryPrivate));
}

static GtkListStore *
get_history_store (GsearchHistoryEntry *entry)
{
	GtkTreeModel *store;

	store = gtk_combo_box_get_model (GTK_COMBO_BOX (entry));
	g_return_val_if_fail (GTK_IS_LIST_STORE (store), NULL);

	return (GtkListStore *) store;
}

static char *
get_history_key (GsearchHistoryEntry *entry)
{
	return g_strdup (entry->priv->history_id);
}

static GSList *
get_history_list (GsearchHistoryEntry *entry)
{
	GtkListStore *store;
	GtkTreeIter iter;
	gboolean valid;
	GSList *list = NULL;

	store = get_history_store (entry);

	valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store),
					       &iter);

	while (valid)
	{
		gchar *str;

		gtk_tree_model_get (GTK_TREE_MODEL (store),
				    &iter,
				    0, &str,
				    -1);

		list = g_slist_prepend (list, str);

		valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (store),
						  &iter);
	}

	return g_slist_reverse (list);
}

static void
gsearch_history_entry_save_history (GsearchHistoryEntry *entry)
{
	GVariant *history;
	GSList *items;
	gchar *key;
	GVariantBuilder item_builder;
	GVariantBuilder history_builder;
	GVariantIter *iter;
	GVariant     *item;
	GVariant     *history_list;
	GSList *list_iter;
	gchar *history_key;

	g_return_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry));

	items = get_history_list (entry);
	key = get_history_key (entry);

	history = g_settings_get_value (entry->priv->settings,
				        "search-history");

	g_variant_builder_init (&item_builder, G_VARIANT_TYPE ("as"));
	for (list_iter = items; list_iter; list_iter = list_iter->next)
		g_variant_builder_add (&item_builder, "s", (gchar *) list_iter->data);

	g_variant_builder_init (&history_builder, G_VARIANT_TYPE ("a{sas}"));
	g_variant_builder_add (&history_builder, "{sas}", key, &item_builder);

	if (history) {
		g_variant_get (history, "a{sas}", &iter);
		while ((item = g_variant_iter_next_value (iter))) {
			g_variant_get (item, "{s@as}", &history_key, &history_list);
			if (g_strcmp0 (history_key, key) != 0)
				g_variant_builder_add (&history_builder, "{s@as}", history_key, history_list);
	 		g_free (history_key);
	 		g_variant_unref (history_list);
	 		g_variant_unref (item);
 		}
 		g_variant_iter_free (iter);
 		g_variant_unref (history);
 	}

	g_settings_set_value (entry->priv->settings,
			      "search-history",
			      g_variant_new ("a{sas}", &history_builder));

	g_slist_foreach (items, (GFunc) g_free, NULL);
	g_slist_free (items);
	g_free (key);
}

static gboolean
remove_item (GtkListStore *store,
	     const gchar  *text)
{
	GtkTreeIter iter;

	g_return_val_if_fail (text != NULL, FALSE);

	if (!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), &iter))
		return FALSE;

	do
	{
		gchar *item_text;

		gtk_tree_model_get (GTK_TREE_MODEL (store),
				    &iter,
				    0,
				    &item_text,
				    -1);

		if (item_text != NULL &&
		    strcmp (item_text, text) == 0)
		{
			gtk_list_store_remove (store, &iter);
			g_free (item_text);
			return TRUE;
		}

		g_free (item_text);

	} while (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter));

	return FALSE;
}

static void
clamp_list_store (GtkListStore *store,
		  guint         max)
{
	GtkTreePath *path;
	GtkTreeIter iter;

	/* -1 because TreePath counts from 0 */
	path = gtk_tree_path_new_from_indices (max - 1, -1);

	if (gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &iter, path))
	{
		while (1)
		{
			if (!gtk_list_store_remove (store, &iter))
				break;
		}
	}

	gtk_tree_path_free (path);
}

static void
insert_history_item (GsearchHistoryEntry *entry,
		     const gchar       *text,
		     gboolean           prepend)
{
	GtkListStore *store;
	GtkTreeIter iter;

	if (g_utf8_strlen (text, -1) <= MIN_ITEM_LEN)
		return;

	store = get_history_store (entry);

	/* remove the text from the store if it was already
	 * present. If it wasn't, clamp to max history - 1
	 * before inserting the new row, otherwise appending
	 * would not work */

	if (!remove_item (store, text))
		clamp_list_store (store,
				  entry->priv->history_length - 1);

	if (prepend)
		gtk_list_store_insert (store, &iter, 0);
	else
		gtk_list_store_append (store, &iter);

	gtk_list_store_set (store,
			    &iter,
			    0,
			    text,
			    -1);

	gsearch_history_entry_save_history (entry);
}

void
gsearch_history_entry_prepend_text (GsearchHistoryEntry *entry,
				    const gchar       *text)
{
	g_return_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry));
	g_return_if_fail (text != NULL);

	insert_history_item (entry, text, TRUE);
}

void
gsearch_history_entry_append_text (GsearchHistoryEntry *entry,
				   const gchar       *text)
{
	g_return_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry));
	g_return_if_fail (text != NULL);

	insert_history_item (entry, text, FALSE);
}

static void
gsearch_history_entry_load_history (GsearchHistoryEntry *entry)
{
	GtkListStore *store;
	GtkTreeIter iter;
	GVariant *history;
	gchar *key;
	gint i;

	g_return_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry));

	store = get_history_store (entry);
	key = get_history_key (entry);

	history = g_settings_get_value (entry->priv->settings,
					"search-history");

	gtk_list_store_clear (store);

	if (history) {
		GVariantIter *history_iter, *history_subiter;
		GVariant     *history_item, *history_subitem;
		gchar        *history_key;
		gchar        *text;

		g_variant_get (history, "a{sas}", &history_iter);

		while ((history_item = g_variant_iter_next_value (history_iter))) {
			i = 0;
			g_variant_get (history_item, "{sas}", &history_key, &history_subiter);

			if (g_strcmp0 (history_key, key) == 0) {
				while ((history_subitem = g_variant_iter_next_value (history_subiter)) &&
				       i < entry->priv->history_length) {
					g_variant_get (history_subitem, "s", &text);
					gtk_list_store_append (store, &iter);
					gtk_list_store_set (store,
							    &iter,
							    0,
							    text,
							    -1);
					g_free (text);
					g_variant_unref (history_subitem);
					i++;
				}
			}
			g_free (history_key);
	 		g_variant_iter_free (history_subiter);
			g_variant_unref (history_item);
 		}
 		g_variant_iter_free (history_iter);
		g_variant_unref (history);
 	}
	g_free (key);
}

void
gsearch_history_entry_clear (GsearchHistoryEntry *entry)
{
	GtkListStore *store;

	g_return_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry));

	store = get_history_store (entry);
	gtk_list_store_clear (store);

	gsearch_history_entry_save_history (entry);
}

static void
gsearch_history_entry_init (GsearchHistoryEntry *entry)
{
	GsearchHistoryEntryPrivate *priv;

	priv = GSEARCH_HISTORY_ENTRY_GET_PRIVATE (entry);
	entry->priv = priv;

	priv->history_id = NULL;
	priv->history_length = GSEARCH_HISTORY_ENTRY_HISTORY_LENGTH_DEFAULT;

	priv->completion = NULL;

	priv->settings = g_settings_new ("org.mate.search-tool");
}

void
gsearch_history_entry_set_history_length (GsearchHistoryEntry *entry,
					guint              history_length)
{
	g_return_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry));
	g_return_if_fail (history_length > 0);

	entry->priv->history_length = history_length;

	/* TODO: update if we currently have more items than max */
}

guint
gsearch_history_entry_get_history_length (GsearchHistoryEntry *entry)
{
	g_return_val_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry), 0);

	return entry->priv->history_length;
}

gchar *
gsearch_history_entry_get_history_id (GsearchHistoryEntry *entry)
{
	g_return_val_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry), NULL);

	return g_strdup (entry->priv->history_id);
}

void
gsearch_history_entry_set_enable_completion (GsearchHistoryEntry *entry,
					     gboolean           enable)
{
	g_return_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry));

	if (enable)
	{
		if (entry->priv->completion != NULL)
			return;

		entry->priv->completion = gtk_entry_completion_new ();
		gtk_entry_completion_set_model (entry->priv->completion,
						GTK_TREE_MODEL (get_history_store (entry)));

		/* Use model column 0 as the text column */
		gtk_entry_completion_set_text_column (entry->priv->completion, 0);

		gtk_entry_completion_set_minimum_key_length (entry->priv->completion,
							     MIN_ITEM_LEN);

		gtk_entry_completion_set_popup_completion (entry->priv->completion, FALSE);
		gtk_entry_completion_set_inline_completion (entry->priv->completion, TRUE);

		/* Assign the completion to the entry */
		gtk_entry_set_completion (GTK_ENTRY (gsearch_history_entry_get_entry(entry)),
					  entry->priv->completion);
	}
	else
	{
		if (entry->priv->completion == NULL)
			return;

		gtk_entry_set_completion (GTK_ENTRY (gsearch_history_entry_get_entry (entry)),
					  NULL);

		g_object_unref (entry->priv->completion);

		entry->priv->completion = NULL;
	}
}

gboolean
gsearch_history_entry_get_enable_completion (GsearchHistoryEntry *entry)
{
	g_return_val_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry), FALSE);

	return entry->priv->completion != NULL;
}

GtkWidget *
gsearch_history_entry_new (const gchar *history_id,
			   gboolean     enable_completion)
{
	GtkWidget *ret;
	GtkListStore *store;

	g_return_val_if_fail(history_id != NULL, NULL);

	/* Note that we are setting the model, so
	 * user must be careful to always manipulate
	 * data in the history through gsearch_history_entry_
	 * functions.
	 */

	store = gtk_list_store_new(1, G_TYPE_STRING);

	ret = g_object_new(GSEARCH_TYPE_HISTORY_ENTRY,
						"has-entry", TRUE,
						"history-id", history_id,
						"model", store,
						"entry-text-column", 0,
						NULL);

	g_object_unref (store);

	/* loading has to happen after the model
	 * has been set. However the model is not a
	 * G_PARAM_CONSTRUCT property of GtkComboBox
	 * so we cannot do this in the constructor.
	 * For now we simply do here since this widget is
	 * not bound to other programming languages.
	 * A maybe better alternative is to override the
	 * model property of combobox and mark CONTRUCT_ONLY.
	 * This would also ensure that the model cannot be
	 * set explicitely at a later time.
	 */
	gsearch_history_entry_load_history (GSEARCH_HISTORY_ENTRY (ret));

	gsearch_history_entry_set_enable_completion (GSEARCH_HISTORY_ENTRY (ret),
						   enable_completion);

	return ret;
}

/*
 * Utility function to get the editable text entry internal widget.
 * I would prefer to not expose this implementation detail and
 * simply make the GsearchHistoryEntry widget implement the
 * GtkEditable interface. Unfortunately both GtkEditable and
 * GtkComboBox have a "changed" signal and I am not sure how to
 * handle the conflict.
 */
GtkWidget *
gsearch_history_entry_get_entry (GsearchHistoryEntry *entry)
{
	g_return_val_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry), NULL);

	return gtk_bin_get_child (GTK_BIN (entry));
}

static void
escape_cell_data_func (GtkTreeViewColumn             *col,
		       GtkCellRenderer               *renderer,
		       GtkTreeModel                  *model,
		       GtkTreeIter                   *iter,
		       GsearchHistoryEntryEscapeFunc  escape_func)
{
	gchar *str;
	gchar *escaped;

	gtk_tree_model_get (model, iter, 0, &str, -1);
	escaped = escape_func (str);
	g_object_set (renderer, "text", escaped, NULL);

	g_free (str);
	g_free (escaped);
}

void
gsearch_history_entry_set_escape_func (GsearchHistoryEntry           *entry,
				       GsearchHistoryEntryEscapeFunc  escape_func)
{
	GList *cells;

	g_return_if_fail (GSEARCH_IS_HISTORY_ENTRY (entry));

	cells = gtk_cell_layout_get_cells (GTK_CELL_LAYOUT (entry));

	/* We only have one cell renderer */
	g_return_if_fail (cells->data != NULL && cells->next == NULL);

	if (escape_func != NULL)
		gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (entry),
						    GTK_CELL_RENDERER (cells->data),
						    (GtkCellLayoutDataFunc) escape_cell_data_func,
						    escape_func,
						    NULL);
	else
		gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (entry),
						    GTK_CELL_RENDERER (cells->data),
						    NULL,
						    NULL,
						    NULL);

	g_list_free (cells);
}
