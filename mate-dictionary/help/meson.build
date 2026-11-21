fs = import('fs')

help_id = 'mate-dictionary'

help_sources = ['index.docbook', 'legal.xml']
help_media = [
  'figures/mate-dictionary-add-source.png',
  'figures/mate-dictionary-applet.png',
  'figures/mate-dictionary-applet-window.png',
  'figures/mate-dictionary-application.png',
  'figures/mate-dictionary-find.png',
  'figures/mate-dictionary-lookup.png',
  'figures/mate-dictionary-preferences-print.png',
  'figures/mate-dictionary-preferences-source.png',
]

langs = []
foreach entry : run_command('ls', '-1', meson.current_source_dir(), check: true).stdout().strip().split('\n')
  entry_path = join_paths(meson.current_source_dir(), entry)
  if entry != 'C' and fs.is_dir(entry_path) and fs.exists(join_paths(entry_path, entry + '.po'))
    langs += entry
  endif
endforeach

gnome.yelp(help_id,
  sources: help_sources,
  media: help_media,
  languages: langs,
  symlink_media: true,
)
