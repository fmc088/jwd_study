
说明：这个是v7.0版本电量计，修复了一些bug和优化了放电效果。

使用：
1. 直接采取文件覆盖的方式更新驱动，包括uboot和kernel；

   其中uboot里给的文件include/power/rk818_pmic.h 最好不要完全覆盖，把fg_rk818.c用到的寄存器定义抄过去就好了，否则担心引起其他的编译问题。
  另外，uboot编译时如果遇到这句话编译报错：parent = fdt_node_offset_by_compatible(blob, g_i2c_node, "rockchip,rk818");，则把g_i2c_node改回文件覆盖前的内容，不影响功能。

2. dts的修改参考pdf文档。（不改也可以，驱动会去兼容旧的，但是能改最好改掉）




=============================================
版本信息，方便回溯。

【kernel】
commit cdb3769dc33cf08773ccc2e53dfddf6139b120f2
Author: Jianhong Chen <chenjh@rock-chips.com>
Date:   Mon Jan 23 15:01:50 2017 +0800

    power: rk818/6-battery: move irq init to the last step
    
    init irq later than workqueue_struct and delayed_work to
    avoid NULL ponint
    
    Change-Id: Ie4bc006d70d0a61b548fcdce86874de44a1dad8b
    Signed-off-by: Jianhong Chen <chenjh@rock-chips.com>


【uboot】
commit 42da0bdcc3bc4caaedc7b5b2509ebb4f07671d77
Author: Jianhong Chen <chenjh@rock-chips.com>
Date:   Tue Nov 8 08:35:15 2016 +0800

    fuel_gauge: rk816/818: init virtual dsoc earlier to avoid overriding real dsoc
    
    Change-Id: Ia7fa10b9f33280868f822aa1f74ed7b1dc653b33
    Signed-off-by: Jianhong Chen <chenjh@rock-chips.com>