开启exfat的支持方式：
1、打上exfat.patch ,方式 patch -p1 < exfat.patch
2、开启exfat文件系统的支持， make menuconfig 进去配置页面，File systems
->DOS/FAT/NT FileSystems->exFat fs Support.
3、从新编译生成kernel，烧写。
