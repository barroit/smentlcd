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
	'custom.css',
]

extensions = [
	'linuxdoc.rstKernelDoc',
]
