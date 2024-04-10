## Dragon server

HTTPS Server based on [https://github.com/yhirose/cpp-httplib](https://github.com/yhirose/cpp-httplib)

## issues

#### how to build openssl-3.0.0 ?

```shell
tar xzvf openssl-3.0.0.tar.gz
cd openssl-3.0.0
mkdir build
cd build
../Configure --prefix=/home/greatwall/work/dragon_server/lib/openssl/openssl_dynamic_3.0.0
make -j8
make test
make install
```

#### link error: DSO missing from command line

**cpp-httplib** depend on openssl (version >=3.0.0) shared library, _NOT static library_ you should add custom openssl lib position as following:

```shell
openssl_dep = meson.get_compiler('cpp').find_library(
        'ssl',
        dirs : join_paths(meson.source_root(),'lib/openssl/openssl_share_3.0.0/lib'),
        version:'>=3.0.0',
        required:get_option('cpp-httplib_openssl')
    )
```

#### link error: lib/libcrypto.so.3: error adding symbols: DSO missing from command

**cpp-httplib** can't found libcrypto.so shared library (version >=3.0.0), you should add it as following in meson.build file

```shell
crypto_dep =  meson.get_compiler('cpp').find_library(
        'crypto',
        dirs : join_paths(meson.source_root(),'lib/openssl/openssl_share_3.0.0/lib'),
        version:'>=3.0.0',
        required:get_option('cpp-httplib_openssl')
    )
```
