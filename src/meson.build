#
# target: libcshquote.so
#

libcshquote_symfile = join_paths(meson.current_source_dir(), 'libcshquote.sym')

libcshquote_deps = [
        dep_cstdaux,
]

libcshquote_private = static_library(
        'cshquote-private',
        [
                'c-shquote.c',
        ],
        c_args: [
                '-fvisibility=hidden',
                '-fno-common',
        ],
        dependencies: libcshquote_deps,
        pic: true,
)

libcshquote_shared = shared_library(
        'cshquote',
        objects: libcshquote_private.extract_all_objects(),
        dependencies: libcshquote_deps,
        install: not meson.is_subproject(),
        soversion: 0,
        link_depends: libcshquote_symfile,
        link_args: [
                '-Wl,--no-undefined',
                '-Wl,--version-script=@0@'.format(libcshquote_symfile),
        ],
)

libcshquote_dep = declare_dependency(
        include_directories: include_directories('.'),
        link_with: libcshquote_private,
        dependencies: libcshquote_deps,
        version: meson.project_version()
)

if not meson.is_subproject()
        install_headers('c-shquote.h')

        mod_pkgconfig.generate(
                libraries: libcshquote_shared,
                version: meson.project_version(),
                name: 'libcshquote',
                filebase: 'libcshquote',
                description: project_description,
        )
endif

#
# target: test-*
#

test_api = executable('test-api', ['test-api.c'], link_with: libcshquote_shared)
test('API Symbol Visibility', test_api)

test_basic = executable('test-basic', ['test-basic.c'], dependencies: libcshquote_dep)
test('Basic API Behavior', test_basic)

test_private = executable('test-private', ['test-private.c'], dependencies: libcshquote_dep)
test('Private Helper Functions', test_private)

if use_reference_test
        test_reference = executable('test-reference', ['test-reference.c'], dependencies: [ libcshquote_dep, dep_glib ])
        test('Reference Tests', test_reference)
endif
