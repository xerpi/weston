srcs_cairo_shared = [
	'cairo-util.c',
	'frame.c',
	'image-loader.c',
]

deps_cairo_shared = [
	dependency('cairo'),
	dependency('libpng'),
	dep_pixman,
	dep_libm,
]

if get_option('image_jpeg')
	deps_cairo_shared += cc.find_library('jpeg')
	config_h.set('HAVE_JPEG', '1')
endif

if get_option('image_webp')
	deps_cairo_shared += dependency('libwebp')
	config_h.set('HAVE_WEBP', '1')
endif

lib_cairo_shared = static_library('cairo-shared',
	srcs_cairo_shared,
	include_directories: include_directories('..'),
	dependencies: deps_cairo_shared,
	install: false)
dep_lib_cairo_shared = declare_dependency(
	link_with: lib_cairo_shared,
	dependencies: deps_cairo_shared)
