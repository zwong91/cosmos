;二级引导器核心入口汇编部分, 加载 GDTR 和 IDTR 寄存器，然后初始化 CPU 相关的寄存器。
%include "ldrasm.inc"
global _start
global realadr_call_entry
global IDT_PTR
extern ldrkrl_entry
[section .text]
[bits 32]
_start:
_entry:
	cli
	lgdt [GDT_PTR]	;加载GDT地址到GDTR寄存器
	lidt [IDT_PTR]	;加载IDT地址到IDTR寄存器
	jmp dword 0x8 :_32bits_mode	;长跳转刷新CS影子寄存器

_32bits_mode:
	mov ax, 0x10	; 数据段选择子(目的)
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
	mov esp,0x90000	;使得栈底指向了0x90000
	call ldrkrl_entry	;调用ldrkrl_entry函数
	xor ebx,ebx
	jmp 0x2000000	;跳转到0x2000000的内存地址
	jmp $

;C 语言代码工作在 32 位保护模式下，BIOS 中断工作在 16 位的实模式
;C 语言环境下调用 BIOS 中断:
; 1. 保存 C 语言环境下的 CPU 上下文 ，
;即保护模式下的所有通用寄存器、段寄存器、程序指针寄存器，栈寄存器，把它们都保存在内存中。
; 2. 切换回实模式，调用 BIOS 中断，把 BIOS 中断返回的相关结果，保存在内存中。
; 3. 切换回保护模式，重新加载第 1 步中保存的寄存器。这样 C 语言代码才能重新恢复执行。
realadr_call_entry:
	pushad		;保存通用寄存器
	push    ds	;保存4个段寄存器
	push    es
	push    fs
	push    gs
	call save_eip_jmp	;调用save_eip_jmp
	pop	gs
	pop	fs
	pop	es
	pop	ds			;恢复4个段寄存器
	popad			;恢复通用寄存器
	ret
save_eip_jmp:
	pop esi				;弹出call save_eip_jmp时保存的eip到esi寄存器中，
	mov [PM32_EIP_OFF],esi		;把eip保存到特定的内存空间中
	mov [PM32_ESP_OFF],esp		;把esp保存到特定的内存空间中
	jmp dword far [cpmty_mode]	;[cpmty_mode]处的数据装入 CS：EIP  0x18：0x1000  0x18段描述符索引GDT 中的 16 位代码段描述符: 0x1000 代表段内的偏移地址
cpmty_mode:
	dd 0x1000
	dw 0x18
	jmp $

GDT_START:
knull_dsc: dq 0
kcode_dsc: dq 0x00cf9a000000ffff ;a-e
kdata_dsc: dq 0x00cf92000000ffff
k16cd_dsc: dq 0x00009a000000ffff ;16位代码段描述符
k16da_dsc: dq 0x000092000000ffff ;16位数据段描述符
GDT_END:

GDT_PTR:
GDTLEN	dw GDT_END-GDT_START-1	;GDT界限
GDTBASE	dd GDT_START

IDT_PTR:
IDTLEN	dw 0x3ff
IDTBAS	dd 0	;这是BIOS中断表的地址和长度
