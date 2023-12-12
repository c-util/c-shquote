project(
        'c-shquote',
        'c',
        default_options: [
                'c_std=c11'
        ],
        license: 'Apache',
        meson_version: '>=0.60.0',
        version: '1.1.0',
)
major = meson.project_version().split('.')[0]
project_description = 'POSIX Shell Compatible Argument Parser'

mod_pkgconfig = import('pkgconfig')

dep_cstdaux = dependency('libcstdaux-1', version: '>=1.5.0')
add_project_arguments(dep_cstdaux.get_variable('cflags').split(' '), language: 'c')

#
# Config: compatability
#
use_reference_test = get_option('reference-test')
if use_reference_test
        dep_glib = dependency('glib-2.0', version: '>=2.50')
endif

subdir('src')

meson.override_dependency('libcshquote-'+major, libcshquote_dep, static: true)
