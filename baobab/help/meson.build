fs = import('fs')

help_id = 'mate-disk-usage-analyzer'

help_sources = ['index.docbook', 'legal.xml']
help_media = [
  'figures/baobab_fullscan.png',
  'figures/baobab_prefs.png',
  'figures/baobab_remote.png',
  'figures/baobab_ringschart1.png',
  'figures/baobab_ringschart2.png',
  'figures/baobab_treemaps.png',
  'figures/baobab_window.png',
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
