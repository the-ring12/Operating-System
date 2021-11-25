# 实验1

key word: GTK

## 实验内容

开发一个图形化界面，并展示效果

## 实验过程：

1. 首先在虚拟机上需要安装 gtk

```sh
# 安装必要文件包
yum install libgnomeui-devel

# 安装必要组件
yum install gtk2 gtk2-devel gtk2-devel-docs

# 安装可选组件
yum install gnome-devel gnome-devel-docs
```

2. 需要在window安装一个 X 服务，我安装的时 XMing [下载链接](https://sourceforge.net/projects/xming/?source=typ_redirect)， 如果虚拟机安装了图形化界面可能不需要这一步（<u>可能，未尝试</u>）

   - 在windows 上的 XMing 安装目录下 X0.hosts (也许是 X1）文件中添加虚拟机的IP

   - 在虚拟机的用户目录下的 .bashrc 文件中添加如下

     ```sh
     # 打开文件
     vi ~/.bashrc
     ```

     

     ```sh
     # 添加如下内容
     export DISPLAY = 192.168.94.1:0.0
     # 192.168.94.1 为window IP，需要修改成你windows 机器的
     ```

   - 添加完成后，需 `source`一下，使其生效

     ```sh
     source ~/.bashrc
     ```

3. 实现的以下小功能

   1. 点击鼠标，会跟据点击区域，字体会放到 左上、右上、左下、右下（开始位置为正中间）；
   2. 并每点击一次鼠标，字体颜色也会随之改变（红、黄、蓝三种循环（<u>可能是这三种，我忘了，你可以运行一下就知道了</u>））；
   3. 字体会随着窗体大小而变化。

4. 编译运行

   ```shell
   gcc -o main main.c `pkg-config --libs --cflags gtk+-2.0`
   
   # 需手动添加库
   ```

## 实验收获

图形化界面的显示需要一个 `X` 协议需要下载相关服务才能运行。

linux 还有三个简单的图形化命令：whiptail、zentity、xdialog（其中有一个也是依赖于 GTK 的）

