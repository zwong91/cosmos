//彭东 @ 2021.01.09
/*
计算机屏幕显示往往是显卡的输出，显卡有很多形式：集成在主板的叫集显，做在 CPU 芯
片内的叫核显，独立存在通过 PCIE 接口连接的叫独显
VESA标准的两种工作模式：字符模式和图形模式, 提供VGABIOS固件程序

字符模式屏幕分成 24 行，每行 80 个字符，把这（24*80）个位置映射到以 0xb8000 地址开
始的内存中，每两个字节对应一个字符（一个字节是字符的 ASCII 码，另一个字节为字符的颜色值）
*/
void _strwrite(char* string)
{
    // 显存起始内存地址, 线性地址 汇编代码已经切换到保护模式
    char* p_strdst = (char*)(0xb8000);
    while (*string)
    {

        *p_strdst = *string++;
        p_strdst += 2;
    }
    return;
}

void printf(char* fmt, ...)
{
    _strwrite(fmt);
    return;
}
