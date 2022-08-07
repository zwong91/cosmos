;GRUB头的汇编部分, 让grub识别, 同时设置C语言运行环境
;初始化 CPU 的寄存器，加载 GDT，切换到 CPU 的保护模式
MBT_HDR_FLAGS	EQU 0x00010003 
;第16位被置为1，load_addr和entry_addr都是有效的，分别对应_start和_entry
;load_addr是引导器二进制文件text段的起始地址，即_start, 


MBT_HDR_MAGIC	EQU 0x1BADB002
MBT2_MAGIC	EQU 0xe85250d6
global _start
extern inithead_entry
[section .text]
[bits 32]
_start:
	jmp _entry
align 4
mbt_hdr:
	dd MBT_HDR_MAGIC
	dd MBT_HDR_FLAGS
	dd -(MBT_HDR_MAGIC+MBT_HDR_FLAGS)
	dd mbt_hdr
	dd _start
	dd 0
	dd 0
	dd _entry
	;
	; multiboot header
	;
ALIGN 8
mbhdr:
	DD	0xE85250D6
	DD	0
	DD	mhdrend - mbhdr
	DD	-(0xE85250D6 + 0 + (mhdrend - mbhdr))
	DW	2, 0
	DD	24
	DD	mbhdr
	DD	_start
	DD	0
	DD	0
	DW	3, 0
	DD	12
	DD	_entry 
	DD      0  
	DW	0, 0
	DD	8
mhdrend:

_entry:
	cli	;关中断

	in al, 0x70
	or al, 0x80	
	out 0x70,al

	lgdt [GDT_PTR]		;加载GDT地址到GDTR寄存器
	jmp dword 0x8 :_32bits_mode	;长跳转刷新CS影子寄存器

;初始化段寄存器和通用寄存器、栈寄存器
_32bits_mode:
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	xor eax,eax
	xor ebx,ebx
	xor ecx,ecx
	xor edx,edx
	xor edi,edi
	xor esi,esi
	xor ebp,ebp
	xor esp,esp
	mov esp,0x7c00	;设置栈顶为0x7c00
	call inithead_entry		;调用inithead_entry函数在inithead.c中实现
	jmp 0x200000		 	;跳转到0x200000物理地址, 进入二级引导器, 模块改变了重新收拾下设置寄存器


;GDT全局段描述符表
GDT_START:
knull_dsc: dq 0
kcode_dsc: dq 0x00cf9e000000ffff
kdata_dsc: dq 0x00cf92000000ffff
k16cd_dsc: dq 0x00009e000000ffff	;16位代码段描述符
k16da_dsc: dq 0x000092000000ffff	;16位数据段描述符
GDT_END:
GDT_PTR:
GDTLEN	dw GDT_END-GDT_START-1	;GDT界限
GDTBASE	dd GDT_START
