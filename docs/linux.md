### 查看网关路由

```
ip route show
删除默认路由
sudo route del default gw 169.254.0.0
添加默认路由
sudo route add default gw 172.26.0.0
```

### ubuntu18.03 安装中文拼音输入法方法步骤

#### 1. 安装Chinese语言包
>先找到Settings（设置）并打开，然后找到Region & Language(区域和语言)并打开，选中Manage Installed Languages，
然后选择Install/Remove Language...，选中Chineses(Simplified)，点击Apply，然后需要稍微等待一会

#### 2. 安装ibus输入法
>中文语言包安装完成后，就要安装ibus输入法了，Terminal（终端）输入命令：
```
sudo apt-get install ibus ibus-clutter ibus-gtk ibus-gtk3 ibus-qt4
```

#### 3. 安装中文拼音引擎
>ibus框架安装完成后，需要再切换到ibus框架
```
im-config -s ibus
```
>切换到ibus框架之后，开始安装中文拼音引擎
```
sudo apt-get install ibus-pinyin
```
#### 4. 添加中文拼音输入法到IBus Preference中
>调出 IBus Preference 窗口
```
sudo ibus-setup
```
>然后依次选择Input Method，Add，Chinese，Pinyin，Add，
(如果这一步找不到Pinyin选项，就重启Ubuntu，再进来就有了) ，最后 Close。

#### 5. 添加中文拼音输入法到键盘输入来源（Input sources）中
>打开Settings（设置），找到Region & Language(区域和语言)并打开，在Input Sources(输入源)下，点击+，选择Chinese，
然后找Chinese(Pinyin)，如果找不到 Chinese(Pinyin) 再重启一次，打开再进来就能找到了,找到Chinese(Pinyin)，点击Add即可。

#### 6. 中英文输入法切换设置
>Ubuntu系统与Windows系统中英文切换不一样，Ubuntu中默认的输入法切换的快捷键为win+space，如果你想要按照自己的习惯重新设置快捷键的话，按以下操作：
先打开设置，然后点击左上角的小放大镜标识，搜索keyboard（键盘），在右侧Typing选项下点击：切换至上个输入源或切换至下个输入源。
然后在键盘上按下你想要的快捷键，点击Set(设置），设置完成。至此所有操作完成，就可以在Ubuntu系统中自如的使用中文拼音输入法了。


### 使用OpenSSL生成自签名证书

#### 1. 创建私钥（.key）

```
openssl genrsa -out dragon.key 2048
```

#### 2. 基于私钥（.key）创建证书签名请求（.csr）

```
openssl req -new -key dragon.key -out dragon.csr -subj "/C=CN/ST=guangdong/L=shenzhen/O=GreatWall/OU=IT/CN=None/CN=Unknown"

```
#### 3. 使用自己的私钥（.key）签署自己的证书签名请求（.csr），生成自签名证书（.crt）

```
openssl x509 -req -in dragon.csr -out dragon.crt -signkey dragon.key -days 3650
```