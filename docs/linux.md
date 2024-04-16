### 查看网关路由

```
ip route show
删除默认路由
sudo route del default gw 169.254.0.0
添加默认路由
sudo route add default gw 172.26.0.0
```

### ubuntu18.03 安装中文拼音输入法方法步骤

#### 1. 安装 Chinese 语言包

> 先找到 Settings（设置）并打开，然后找到 Region & Language(区域和语言)并打开，选中 Manage Installed Languages，
> 然后选择 Install/Remove Language...，选中 Chineses(Simplified)，点击 Apply，然后需要稍微等待一会

#### 2. 安装 ibus 输入法

> 中文语言包安装完成后，就要安装 ibus 输入法了，Terminal（终端）输入命令：

```
sudo apt-get install ibus ibus-clutter ibus-gtk ibus-gtk3 ibus-qt4
```

#### 3. 安装中文拼音引擎

> ibus 框架安装完成后，需要再切换到 ibus 框架

```
im-config -s ibus
```

> 切换到 ibus 框架之后，开始安装中文拼音引擎

```
sudo apt-get install ibus-pinyin
```

#### 4. 添加中文拼音输入法到 IBus Preference 中

> 调出 IBus Preference 窗口

```
sudo ibus-setup
```

> 然后依次选择 Input Method，Add，Chinese，Pinyin，Add，
> (如果这一步找不到 Pinyin 选项，就重启 Ubuntu，再进来就有了) ，最后 Close。

#### 5. 添加中文拼音输入法到键盘输入来源（Input sources）中

> 打开 Settings（设置），找到 Region & Language(区域和语言)并打开，在 Input Sources(输入源)下，点击+，选择 Chinese，
> 然后找 Chinese(Pinyin)，如果找不到 Chinese(Pinyin) 再重启一次，打开再进来就能找到了,找到 Chinese(Pinyin)，点击 Add 即可。

#### 6. 中英文输入法切换设置

> Ubuntu 系统与 Windows 系统中英文切换不一样，Ubuntu 中默认的输入法切换的快捷键为 win+space，如果你想要按照自己的习惯重新设置快捷键的话，按以下操作：
> 先打开设置，然后点击左上角的小放大镜标识，搜索 keyboard（键盘），在右侧 Typing 选项下点击：切换至上个输入源或切换至下个输入源。
> 然后在键盘上按下你想要的快捷键，点击 Set(设置），设置完成。至此所有操作完成，就可以在 Ubuntu 系统中自如的使用中文拼音输入法了。

### 使用 OpenSSL 生成自签名证书

#### 1. 创建私钥（.key）

```
openssl genrsa -out dragon.key 2048
```

#### 2. 基于私钥（.key）创建证书签名请求（.csr）

```
openssl req -new -key dragon.key -out dragon.csr -subj "/C=CN/ST=guangdong/L=shenzhen/O=Greatwall/OU=IT/CN=localhost/emailAddress=shb8845369@gmail.com"

```

#### 3. 使用自己的私钥（.key）签署自己的证书签名请求（.csr），生成自签名证书（.crt）

```
openssl x509 -req -in dragon.csr -out dragon.crt -signkey dragon.key -days 3650
```
