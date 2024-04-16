project('dragon https server', 'cpp',
         version:'1.0.0',
         license: 'MIT',
         default_options: [
            'cpp_std=c++11',
            'buildtype=release',
            'b_ndebug=if-release',
            'b_lto=true',
            'warning_level=3',
            'default_library=static'
         ],
         meson_version: '>=0.47.0')

# cpp_args ='-Werror'

compile_time = run_command('date', '+%F %H:%M:%S').stdout().strip()

#################### http-lib 库编译依赖OpenSSL3.0.0以上版本 #######################
deps = [dependency('threads')]
args = []

includes = include_directories('lib/openssl/openssl_share_3.0.0/include')

# openssl_dep = dependency('openssl', version: '>=3.0.0', required: get_option('cpp-httplib_openssl'))
# 自定义openssl版本共享库路径
openssl_dep = meson.get_compiler('cpp').find_library(
        'ssl',
        dirs : join_paths(meson.source_root(),'lib/openssl/openssl_share_3.0.0/lib'),
        required:get_option('cpp-httplib_openssl')
    )
crypto_dep =  meson.get_compiler('cpp').find_library(
        'crypto',
        dirs : join_paths(meson.source_root(),'lib/openssl/openssl_share_3.0.0/lib'),
        required:get_option('cpp-httplib_openssl')
    )
if openssl_dep.found()
  deps += openssl_dep
  deps += crypto_dep
  args += '-DCPPHTTPLIB_OPENSSL_SUPPORT'
  if host_machine.system() == 'darwin'
    macosx_keychain_dep = dependency('appleframeworks', modules: ['CoreFoundation', 'Security'], required: get_option('cpp-httplib_macosx_keychain'))
    if macosx_keychain_dep.found()
        deps += macosx_keychain_dep
        args += '-DCPPHTTPLIB_USE_CERTS_FROM_MACOSX_KEYCHAIN'
    endif  
  endif
endif

zlib_dep = dependency('zlib', required: get_option('cpp-httplib_zlib'))
if zlib_dep.found()
  deps += zlib_dep
  args += '-DCPPHTTPLIB_ZLIB_SUPPORT'
endif

brotli_deps = [dependency('libbrotlicommon', required: get_option('cpp-httplib_brotli'))]
brotli_deps += dependency('libbrotlidec',    required: get_option('cpp-httplib_brotli'))
brotli_deps += dependency('libbrotlienc',    required: get_option('cpp-httplib_brotli'))

brotli_found_all = true
foreach brotli_dep : brotli_deps
  if not brotli_dep.found()
    brotli_found_all = false
  endif
endforeach

if brotli_found_all
  deps += brotli_deps
  args += '-DCPPHTTPLIB_BROTLI_SUPPORT'
endif

cpp_httplib_dep = dependency('', required: false)

if get_option('cpp-httplib_compile')
  python3 = find_program('python3')

  httplib_ch = custom_target(
    'split',
    input: 'httplib.h',
    output: ['httplib.cc', 'httplib.h'],
    command: [python3, files('split.py'), '--out', meson.current_build_dir()],
    install: true,
    install_dir: [false, get_option('includedir')]
  )
  lib = library(
    'cpp-httplib',
    sources: httplib_ch,
    dependencies: deps,
    cpp_args: args,
    version: version,
    soversion: version.split('.')[0] + '.' + version.split('.')[1],
    install: true
  )
  cpp_httplib_dep = declare_dependency(compile_args: args, dependencies: deps, link_with: lib, sources: httplib_ch[1])

  import('pkgconfig').generate(
    lib,
    description: 'A C++ HTTP/HTTPS server and client library',
    extra_cflags: args,
    url: 'https://github.com/yhirose/cpp-httplib',
    version: version
  )
else
#   install_headers('httplib.h')
  cpp_httplib_dep = declare_dependency(compile_args: args, dependencies: deps, include_directories: include_directories('.'))
#   import('pkgconfig').generate(
#     name: 'cpp-httplib',
#     description: 'A C++ HTTP/HTTPS server and client library',
#     install_dir: join_paths(get_option('datadir'), 'pkgconfig'),
#     url: 'https://github.com/yhirose/cpp-httplib',
#     version: version
#   )
endif

if meson.version().version_compare('>=0.54.0')
  meson.override_dependency('cpp-httplib', cpp_httplib_dep)
endif

if get_option('cpp-httplib_test')
  subdir('test')
endif

###############################################################

# 拷贝配置文件到build目录
config_files = [
    'dragon.crt',
    'dragon.key',
    'dragon.ini'
]

foreach file : config_files
    configure_file(input : './src/config/'+file,
    output : file,
    copy: true)
endforeach

sources = [
    'src/App.cpp',
    'src/DragonServer.cpp',
    'src/EasyLogger.cpp',
    'src/Request.cpp',
    'src/FileUtils.cpp',
    'src/ConsoleTable.cpp',
    'src/Base64.cpp',
    'src/Global.cpp',
    'lib/jsoncpp.cpp']

executable('dragon', 
            sources,
            # cpp_args:cpp_args,
            link_args : ['-ldl'],
            dependencies:cpp_httplib_dep,
            include_directories:includes)