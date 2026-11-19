# SPDX-License-Identifier: GPL-3.0-or-later

copyright = '2026 Jiamu Sun'
author = 'Jiamu Sun'

project = 'smentlcd'
version = 'v0.1'
release = version

needs_sphinx = '9.1.0'
smartquotes_action = 'q'

html_theme = 'furo'
html_logo = 'images/strawberry.svg'
html_favicon = 'images/strawberry.png'

html_static_path = [
	'static',
]

html_css_files = [
	'dark_mode.css',
	'code_color.css',
	'code_tab.css',
	'layout.css',
]

extensions = [
	'linuxdoc.rstKernelDoc',
	'sphinx.ext.autosectionlabel',
	'sphinx_copybutton',
	'sphinx_tabs.tabs',
]

kernel_doc_srctree = '..'

autosectionlabel_prefix_document = True
autosectionlabel_maxdepth = 2

sphinx_tabs_disable_css_loading = True
sphinx_tabs_disable_tab_closing = True
