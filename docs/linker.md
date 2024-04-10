### 编译高版本 OpenSSL 3.1.5 静态库

```
tar xzvf openssl-3.1.5.tar.gz
cd openssl-3.1.5
mkdir build
cd build
../Configure --prefix=/home/greatwall/work/dragon_server/lib/openssl/openssl_static_3.1.5 no-shared
make -j8
make test
make install
```

### 编译高版本 OpenSSL 3.0.13 动态库

```
tar xzvf openssl-3.0.13.tar.gz
cd openssl-3.0.13
mkdir build
cd build
../Configure --prefix=/home/greatwall/work/dragon_server/lib/openssl/openssl_dynamic_3.0.13
make -j8
make test
make install
```

### 设置 vscode cpp include 路径

> g++ -v -E -x c++ -
> ctrl+shift+p | json

```
"/usr/include/c++/9",
"/usr/include/aarch64-linux-gnu/c++/9"
"/usr/include/c++/9/backward",
"/usr/lib/gcc/aarch64-linux-gnu/9/include",
"/usr/local/include",
"/usr/include/aarch64-linux-gnu",
"/usr/include"
```

> 重启 VSCODE

### ubuntu 可执行文件段错误（已存储），如何使用 gdb 调试?

```
1. 查看段错误（已存储）转储文件保存大小限制
ulimit -c unlimited

2. 查看段错误（已存储）保存的文件路径
cat /proc/sys/kernel/core_pattern

3. 自定义段错误转储文件存储路径
// 控制core文件的文件名中是否添加pid作为扩展
echo "1" > /proc/sys/kernel/core_uses_pid
// 设置core文件的输出路径和输出文件名，这里我的路径是/home/boy/corefile，文件名就是后面的部分
echo "/home/boy/corefile/core-%e-%p-%t"> /proc/sys/kernel/core_pattern

//参数说明
%p - insert pid into filename 添加pid
%u - insert current uid into filename 添加当前uid
%g - insert current gid into filename 添加当前gid
%s - insert signal that caused the coredump into the filename 添加导致产生core的信号
%t - insert UNIX time that the coredump occurred into filename 添加core文件生成时的unix时间
%h - insert hostname where the coredump happened into filename 添加主机名
%e - insert coredumping executable name into filename 添加程序名
```

### 如何查看动态库和静态库是 32 位，还是 64 位下的库

```shell
file .so
objdump -x .a
```

### 使用 nm 工具，查看静态库和动态库中有那些函数名

```shell
nm /lib/libxtables.so.10
```

nm 列出的符号有很多， 常见的有三种：

1. T 类：是在库中定义的函数，用 T 表示，这是最常见的
2. U 类：是在库中被调用，但并没有在库中定义（表明需要其他库支持），用 U 表示
3. W 类：是所谓的 弱态 符号，它们虽然在库中被定义，但是可能被其他库中的同名符号覆盖，用 W 表示

### 如何指定动态链接库查找路径和动态库链接名称

```
gcc test.c -L. -lmax
```

生成可执行文件 a.out

1. -L. 表示搜索要链接的库文件时包含当前路径
2. -lmax 表示要链接 libmax.so，如果同一目录下同时存在同名的动态库和静态库，比如 libmax.so 和 libmax.a 都在当前路径下，则 gcc 会优先链接动态库

#### 运行报错

```
$ ./a.out
./a.out: error while loading shared libraries: libmax.so: cannot open shared object file: No such file or directory
```

原因解释: 找不到 libmax.so，原来 Linux 是通过 /etc/ld.so.cache 文件搜寻要链接的动态库的。而 /etc/ld.so.cache 是 ldconfig 程序读取 /etc/ld.so.conf 文件生成的。（ /etc/ld.so.conf 中并不必包含 /lib 和 /usr/lib， ldconfig 程序会自动搜索这两个目录）
如果我们把 libmax.so 所在的路径添加到 /etc/ld.so.conf 中，再以 root 权限运行 ldconfig 程序，更新 /etc/ld.so.cache， a.out 运行时，就可以找到 libmax.so

#### 混合链接

当 ld 使用了选项 -static 时会导致所有的库使用静态链接，因此当使用混合链接是一定不能使用 -static，只能通过 -Wl,-Bstatic -llibname 或 -Wl,-Bdynamic -llibname 来指定链接方式

LIBRARY_PATH 环境变量：指定程序静态链接库文件搜索路径
LD_LIBRARY_PATH 环境变量：指定程序动态链接库文件搜索路径
