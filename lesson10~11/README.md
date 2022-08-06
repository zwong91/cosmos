# 设置工作模式和环境

GRUB -> 二级引导器 -> 硬件抽象层hal初始化

## 内核文件封装打包问题?
GRUB 加载一个内核映像文件, 里面封装了二级引导器模块，内核模块，图片和字库文件，这个映像文件要能被grub识别加载解析内容，约束内核映像文件格式 4KB GRUB头 | 映像文件头描述符 | 文件头描述符... | 文件...

映像打包工具
lmoskrlimg -m k -lhf GRUB头文件 -o 映像文件 -f 输入的文件列表
-m 表示模式 只能是k内核模式
-lhf 表示后面跟上GRUB头文件
-o 表示输出的映像文件名
-f 表示输入文件列表

# VirtualBox 虚拟机 手动建立虚拟硬盘，对其格式化，在其中手动安装 GRUB 引导器
## 制作虚拟硬盘, 大多数虚拟机也是通过文件来模拟硬盘
dd bs=512 if=/dev/zero of=hd.img count=204800  # 100MB 全0数据
;bs:表示块大小，这里是512字节
;if：表示输入文件，/dev/zero就是Linux下专门返回0数据的设备文件，读取它就返回0
;of：表示输出文件，即我们的硬盘文件。
;count：表示输出多少块

### 格式化虚拟硬盘, 从裸盘到建立ext4文件系统, 只要Linux和GRUB能识别, linux可以向其中写入文件（我们的内核）
sudo losetup /dev/loop0 hd.img   # 将 hd.img 变成Linux 的回环设备
sudo mkfs.ext4 -q /dev/loop0

sudo mount -o loop ./hd.img ./hdisk/ ;挂载硬盘文件
sudo mkdir ./hdisk/boot/ ;建立boot目录

# 安装grub
sudo grub-install --boot-directory=./hdisk/boot/ --force --allow-floppy /dev/loop0
；--boot-directory 指向先前我们在虚拟硬盘中建立的boot目录。
；--force --allow-floppy ：指向我们的虚拟硬盘设备文件/dev/loop0

/hdisk/boot/grub/grub.cfg
```
menuentry 'HelloOS' {
insmod part_msdos
insmod ext2
set root='hd0,msdos1' #我们的硬盘只有一个分区所以是'hd0,msdos1'
multiboot2 /boot/HelloOS.eki #加载boot目录下的HelloOS.eki文件
boot #引导启动
}
set timeout_style=menu
if [ "${timeout}" = 0 ]; then
set timeout=10 #等待10秒钟自动启动
fi
}
```
# 转换虚拟硬盘格式
VBoxManage convertfromraw ./hd.img --format VDI ./hd.vdi
;convertfromraw 指向原始格式文件
；--format VDI 表示转换成虚拟需要的VDI格式


# 挂上去虚拟硬盘
#第一步 SATA的硬盘其控制器是intelAHCI
VBoxManage storagectl HelloOS --name "SATA" --add sata --controller IntelAhci
#第二步
VBoxManage closemedium disk ./hd.vdi #删除虚拟硬盘UUID并重新分配
#将虚拟硬盘挂到虚拟机的硬盘控制器
VBoxManage storageattach HelloOS --storagectl "SATA" --port 1 --device 0 --type
https://blog.csdn.net/ll15982534415/article/details/118771286



# grub -> 二级引导器
二级引导器作为操作系统的先驱, 解析内核映像文件， 收集机器环境信息， 对CPU 内存， 显卡初步设置


