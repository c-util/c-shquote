project(
        'c-shquote',
        'c',
        version: '1',
        license: 'Apache',
        default_options: [
                'c_std=c11'
        ],
)
project_description = 'POSIX Shell Compatible Argument Parser'

add_project_arguments('-D_GNU_SOURCE', language: 'c')
mod_pkgconfig = import('pkgconfig')

sub_cstdaux = subproject('c-stdaux')

dep_cstdaux = sub_cstdaux.get_variable('libcstdaux_dep')

#
# Config: compatability
#
use_reference_test = get_option('reference-test')
if use_reference_test
        dep_glib = dependency('glib-2.0', version: '>=2.50')
endif

subdir('src')
