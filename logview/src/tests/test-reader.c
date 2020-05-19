#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../logview-log.h"
#include "../logview-utils.h"

#include <glib.h>
#include <gio/gio.h>

static GMainLoop *loop;

static void
new_lines_cb (LogviewLog *log,
              const char **lines,
              GSList *new_days,
              GError *error,
              gpointer user_data)
{
  int i;
  guint8 day;
  Day *day_s;
  GSList *days, *l;

  for (i = 0; lines[i]; i++) {
    g_print ("line %d: %s\n", i, lines[i]);
  }
  g_print ("outside read, lines no %u\n", logview_log_get_cached_lines_number (log));

  days = log_read_dates (lines, logview_log_get_timestamp (log));
  g_print ("\ndays %p\n", days);

  for (l = days; l; l = l->next) {
    day_s = l->data;
    g_print ("\nday %u month %u\n", g_date_get_day (day_s->date), g_date_get_month (day_s->date));
  }

  g_object_unref (log);

  g_main_loop_quit (loop);
}

static void
callback (LogviewLog *log,
          GError *error,
          gpointer user_data)
{
  g_print ("callback! err %p, log %p\n", error, log);

  logview_log_read_new_lines (log, NULL, new_lines_cb, NULL);
}

int main (int argc, char **argv)
{
  GError *error = NULL;
  gchar *log_filename = NULL;
  gchar *usage;
  GOptionContext *context;
  GOptionEntry entries [] =
    { { "file", 'f', 0, G_OPTION_ARG_FILENAME, &log_filename, "The log file, e.g. /var/log/dpkg.log.2.gz", NULL },
      { NULL } };

  context = g_option_context_new (NULL);
  g_option_context_add_main_entries (context, entries, NULL);

  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    if (error) {
      g_printerr ("%s\n\n", error->message);
      g_error_free (error);
    }
    goto arg_error;
  }

  if (!log_filename) {
    g_printerr ("ERROR: You must specify the log file.\n\n");
    goto arg_error;
  }

  g_option_context_free (context);

  loop = g_main_loop_new (NULL, FALSE);
  logview_log_create (log_filename, callback, NULL);

  g_main_loop_run (loop);

  g_main_loop_unref (loop);

  g_free (log_filename);

  return 0;

arg_error:
  usage = g_option_context_get_help (context, TRUE, NULL);
  g_printerr ("%s", usage);
  g_free (usage);
  g_option_context_free (context);

  return 1;
}
