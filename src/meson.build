#
# target: libcshquote.so
#

libcshquote_symfile = join_paths(meson.current_source_dir(), 'libcshquote.sym')

libcshquote_deps = [
        dep_cstdaux,
]

libcshquote_both = both_libraries(
        'cshquote-'+major,
        [
                'c-shquote.c',
        ],
        c_args: [
                '-fvisibility=hidden',
                '-fno-common',
        ],
        dependencies: libcshquote_deps,
        install: not meson.is_subproject(),
        link_args: [
                '-Wl,--version-script=@0@'.format(libcshquote_symfile),
        ],
        link_depends: libcshquote_symfile,
        soversion: 0,
)

libcshquote_dep = declare_dependency(
        dependencies: libcshquote_deps,
        include_directories: include_directories('.'),
        link_with: libcshquote_both.get_static_lib(),
        version: meson.project_version()
)

if not meson.is_subproject()
        install_headers('c-shquote.h')

        mod_pkgconfig.generate(
                description: project_description,
                filebase: 'libcshquote-'+major,
                libraries: libcshquote_both.get_shared_lib(),
                name: 'libcshquote',
                version: meson.project_version(),
        )
endif

#
# target: test-*
#

test_api = executable('test-api', ['test-api.c'], link_with: libcshquote_both.get_shared_lib())
test('API Symbol Visibility', test_api)

test_basic = executable('test-basic', ['test-basic.c'], dependencies: libcshquote_dep)
test('Basic API Behavior', test_basic)

test_private = executable('test-private', ['test-private.c'], dependencies: libcshquote_dep)
test('Private Helper Functions', test_private)

if use_reference_test
        test_reference = executable('test-reference', ['test-reference.c'], dependencies: [ libcshquote_dep, dep_glib ])
        test('Reference Tests', test_reference)
endif
