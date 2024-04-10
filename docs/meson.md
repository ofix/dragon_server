### 安装 meson 编译环境

> Meson 是一个旨在快速构建的编译系统，可以在多平台上构建，支持 Linux、Windows 和 MacOS。不仅支持编译 C/C++，还支持 D，Fortran，Java，Rust 等语言。
> Meson 基于 Python3 实现，并依赖 Ninja。Meson 和 Ninja 相互配合。Meson 负责构建项目依赖关系，Ninja 进行编译。Ninja 也是一个轻量级的编译系统，后面找机会再介绍下 Ninja。

```
// 通过apt方式也可以安装，只有由于源的关系，安装的版本可能比较低，所以还是建议使用pip3的方式安装。
sudo apt-get install ninja-build
pip3 install ninja
pip3 install meson

// 设置编译目录（builddir是目录名，编译产生的所有文件都会自动存放在这个目录中)
meson setup builddir

// 进入编译目录中编译代码
cd builddir
ninja && ninja install
// 或者
cd builddir
meson compile && ninja install

```

### meson 学习心得

1. meson 如果需要支持交叉编译，设置的环境变量是不会生效的， 需要通过 cross file 来完成配置选项的设置
2. meson 默认使用 ninja 完成真正的程序编译，如果编译需要使用其他的编译器比如微软的 vs 编译器，需要使用如下命令

```shell
meson setup build --backend=vs
```

3. 开始编译，可以使用如下命令

```shell
// 第一种方式
ninja -C build
// 第二种方式
cd build
ninja
```

4. 使用`meson setup build` 之后，源代码的任何文件变动，都无需再重复执行该命令，当然 meson.build 文件要同步调整
5. 如果需要测试代码，请执行以下命令

```shell
// 第一种方式
meson test -C build
// 第二种方式
ninja -C build test
// 第三种方式
cd build
ninja test
```

6. 编译完成后，meson 默认拷贝程序到`/usr/local`目录中，如果需要拷贝编译后的程序到指定目录，需要执行以下命令

```shell
DESTDIR=/path/to/staging
// 第一种方式
meson install -C build
// 第二种方式
ninja -C build install
```

7. 查看当前 meson 项目的所有配置项

```python
meson configure build

// 输出如下
Core properties:
  Source dir /home/greatwall/work/dragon_server
  Build dir  /home/greatwall/work/dragon_server/build

Main project options:

  Core options        Current Value         Possible Values                                               Description
  ------------        -------------         ---------------                                               -----------
  auto_features       auto                  [enabled, disabled, auto]                                     Override value of all 'auto' features
  backend             ninja                 [ninja, vs, vs2010, vs2015, vs2017, vs2019, xcode]            Backend to use
  buildtype           debug                 [plain, debug, debugoptimized, release, minsize, custom]      Build type to use
  debug               true                  [true, false]                                                 Debug
  default_library     static                [shared, static, both]                                        Default library type
  install_umask       0022                  [preserve, 0000-0777]                                         Default umask to apply on permissions of installed files
  layout              mirror                [mirror, flat]                                                Build directory layout
  optimization        0                     [0, g, 1, 2, 3, s]                                            Optimization level
  strip               false                 [true, false]                                                 Strip targets on install
  unity               off                   [on, off, subprojects]                                        Unity build
  warning_level       1                     [0, 1, 2, 3]                                                  Compiler warning level to use
  werror              false                 [true, false]                                                 Treat warnings as errors
  wrap_mode           default               [default, nofallback, nodownload, forcefallback]              Wrap mode
  cmake_prefix_path   []                                                                                  T.List of additional prefixes for cmake to search
  pkg_config_path     []                                                                                  T.List of additional paths for pkg-config to search

  Backend options     Current Value         Possible Values                                               Description
  ---------------     -------------         ---------------                                               -----------
  backend_max_links   0                     >=0                                                           Maximum number of linker processes to run or 0 for no limit

  Base options        Current Value         Possible Values                                               Description
  ------------        -------------         ---------------                                               -----------
  b_asneeded          true                  [true, false]                                                 Use -Wl,--as-needed when linking
  b_colorout          always                [auto, always, never]                                         Use colored output
  b_coverage          false                 [true, false]                                                 Enable coverage tracking.
  b_lto               false                 [true, false]                                                 Use link time optimization
  b_lundef            true                  [true, false]                                                 Use -Wl,--no-undefined when linking
  b_ndebug            false                 [true, false, if-release]                                     Disable asserts
  b_pch               true                  [true, false]                                                 Use precompiled headers
  b_pgo               off                   [off, generate, use]                                          Use profile guided optimization
  b_pie               false                 [true, false]                                                 Build executables as position independent
  b_sanitize          none                  [none, address, thread, undefined, memory, address,undefined] Code sanitizer to use
  b_staticpic         true                  [true, false]                                                 Build static libraries as position independent

  Compiler options    Current Value         Possible Values                                               Description
  ----------------    -------------         ---------------                                               -----------
  cpp_args            []                                                                                  Extra arguments passed to the cpp compiler
  cpp_debugstl        false                 [true, false]                                                 STL debug mode
  cpp_eh              default               [none, default, a, s, sc]                                     C++ exception handling type.
  cpp_link_args       []                                                                                  Extra arguments passed to the cpp linker
  cpp_rtti            true                  [true, false]                                                 Enable RTTI
  cpp_std             none                  [none, c++98, c++03, c++11, c++14, c++17, c++1z, c++2a,       C++ language standard to use
                                             gnu++03, gnu++11, gnu++14, gnu++17, gnu++1z, gnu++2a]

  Directories         Current Value         Possible Values                                               Description
  -----------         -------------         ---------------                                               -----------
  bindir              bin                                                                                 Executable directory
  datadir             share                                                                               Data file directory
  includedir          include                                                                             Header file directory
  infodir             share/info                                                                          Info page directory
  libdir              lib/aarch64-linux-gnu                                                               Library directory
  libexecdir          libexec                                                                             Library executable directory
  localedir           share/locale                                                                        Locale data directory
  localstatedir       /var/local                                                                          Localstate data directory
  mandir              share/man                                                                           Manual page directory
  prefix              /usr/local                                                                          Installation prefix
  sbindir             sbin                                                                                System executable directory
  sharedstatedir      /var/local/lib                                                                      Architecture-independent data directory
  sysconfdir          etc                                                                                 Sysconf data directory

  Testing options     Current Value         Possible Values                                               Description
  ---------------     -------------         ---------------                                               -----------
  errorlogs           true                  [true, false]                                                 Whether to print the logs from failing tests
  stdsplit            true                  [true, false]                                                 Split stdout and stderr in test logs


// 如果需要修改以上输出的某个设置项，执行以下命令
meson configure build -Doption=new_value

```

8. 如果编译链接出现奇怪的问题，可以尝试清除编译中间文件，使用如下命令

```shell
meson setup --clearcache --reconfigure <builddir>
```

9. meson 字符串变量和整型变量互转

```python
// 字符串变量转换成正型变量
string_var.to_int()
// 正型变量转换成字符串变量
int_var.to_string()
```

10. meson 字符串变量是以单引号进行包裹的,例如

```python
test='This is a text'
```

11. meson 简单拼接路径的方法

```python
joined = '/usr/share' / 'projectname'    # => /usr/share/projectname
joined = '/usr/local' / '/etc/name'      # => /etc/name

joined = 'C:\\foo\\bar' / 'builddir'     # => C:/foo/bar/builddir
joined = 'C:\\foo\\bar' / 'D:\\builddir' # => D:/builddir
```

12. meson 输入多行文本的优雅方式

```python
multiline_string = '''#include <foo.h>
int main (int argc, char ** argv) {
  return FOO_SUCCESS;
}'''
```

13. meson 字符串格式化采用占位符替换的形式

```python
template = 'string: @0@, number: @1@, bool: @2@'
res = template.format('text', 1, true)
# res now has value 'string: text, number: 1, bool: true'
```

14. meson 字符串移除前后的空格和换行符

```python
# Similar to the Python str.strip(). Removes leading/ending spaces and newlines.
define = ' -Dsomedefine '
stripped_define = define.strip()
# 'stripped_define' now has the value '-Dsomedefine'

# You may also pass a string to strip, which specifies the set of characters to
# be removed instead of the default whitespace.
string = 'xyxHelloxyx'.strip('xy')
# 'string' now has the value 'Hello'
```

15. meson 内置函数说明

```python
// 获取当前项目版本后，如果有设置的话
meson.project_version()
// 将字符串中的非英文字母和数字替换成下划线
name = 'Meson Docs.txt#Reference-manual'
# Replaces all characters other than `a-zA-Z0-9` with `_` (underscore)
# Useful for substituting into #defines, filenames, etc.
underscored = name.underscorify()
# underscored now has the value 'Meson_Docs_txt_Reference_manual'

// meson版本比较有内置函数
version = '1.2.3'
# Compare version numbers semantically
is_new = version.version_compare('>=2.0')
# is_new now has the boolean value false
# Supports the following operators: '>', '<', '>=', '<=', '!=', '==', '='

// 检查某个数组是否包含某个数值
my_array = [1, 2]
if 1 in my_array
# This condition is true
endif
if 1 not in my_array
# This condition is false
endif

// 给函数传递参数，也可以采用字典这种方式
d = {'sources': 'prog.c',
  'c_args': '-DFOO=1'}

executable('progname',
  kwargs: d)

```

16. meson 不支持用户自定义函数和对象
