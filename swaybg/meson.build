executable(
	'swaybg',
	'main.c',
	include_directories: [sway_inc],
	dependencies: [
		cairo,
		client_protos,
		gdk_pixbuf,
		jsonc,
		pango,
		pangocairo,
		wayland_client
	],
	link_with: [lib_sway_common, lib_sway_client],
	install: true
)
