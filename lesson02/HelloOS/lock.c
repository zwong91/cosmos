
/*
锁:  并发数据竞争
1. 原子变量 atomic, 适用简单单体变量  => lock 前缀的 addl、subl、incl、decl 指令都是原子操作，lock 前缀表示锁定总线  volatile
2. 中断控制cli、sti 指令, CPU 的 eflags 寄存器的 IF 位清除和设置
3. 自旋锁 协调多核心 CPU, 保证读取锁变量和判断并加锁的操作是原子执行 , xchg指令，让寄存器里的一个值跟内存空间中的一个值做交换动作, 不受其它 CPU 干扰
4. 信号量 时间管理大师， 无论是原子操作，还是自旋锁，都不适合长时间等待， 等待、互斥、唤醒（即重新激活等待的代码执行流）
*/


// Linux 原子变量、中断控制、自旋锁、信号量、 读写锁  五种数据同步机制， 解决数据竞争和一致性

// 一个原子类型变量 atomic_t

typedef struct
{
    int counter;
} atomic_t; //常用的32位的原子变量类型
#ifdef CONFIG_64BIT
typedef struct
{
    s64 counter;
} atomic64_t; // 64位的原子变量类型
#endif

//原子读取变量中的值
static __ always_inline int arch_atomic_read(const atomic_t *v)
{
    return __READ_ONCE((v)->counter);
}
//原子写入一个具体的值
static __
    always_inline void
    arch_atomic_set(atomic_t *v, int i)
{
    __WRITE_ONCE(v->counter, i);
}
//原子加上一个具体的值
static __always_inline void arch_atomic_add(int i, atomic_t *v)
{
    asm volatile(LOCK_PREFIX "addl %1,%0":" +m" (v->counter)
    :
    "
    ir" (i) : "memory");
}
//原子减去一个具体的值
static __always_inline void arch_atomic_sub(int i, atomic_t *v)
{
    asm volatile(LOCK_PREFIX "subl %1,%0"
    :
    "
    +m" (v->counter)
    :
    "
    ir" (i) : "memory");
}
//原子加1
static __
    always_inline void
    arch_atomic_inc(atomic_t *v)
{
asm volatile(LOCK_PREFIX "incl %0"
:
"
+m" (v->counter) :: "memory");
}
//原子减1
static __
    always_inline void
    arch_atomic_dec(atomic_t *v)
{
asm volatile(LOCK_PREFIX "decl %0"
:
"
+m" (v->counter) :: "memory");
}

#define
__
READ
_
ONCE(x) \
(*(const volatile __unqual_scalar_typeof(x) *)&(x))
#define
__
WRITE
_
ONCE(x, val) \
do
{
    *(volatile typeof(x) *)&(x) = (val);
}
while (0)
//__unqual_scalar_typeof表示声明一个非限定的标量类型，非标量类型保持不变。说人话就是返回x
//如果 x 是int类型则返回“int”
#define
    __READ_ONCE(x)
(*(const volatile int *)&(x))
#define
    __
    WRITE_ONCE(x, val) do
{
    *(volatile int *)&(x) = (val);
}
while (0)

    // Linux 控制中断

    //实际保存eflags寄存器
    extern __
        always_inline unsigned long
        native_save_fl(void)
    {
        unsigned long flags;
asm volatile("# __raw_save_flags\n\t"
"
pushf ; pop %0":"=rm"(flags)::"memory");
return flags;
    }
//实际恢复eflags寄存器
extern inline void native
    _
        restore
            _
            fl(unsigned long flags)
{
    asm volatile("push %0 ; popf" ::"g"(flags)
                 : "memory", "cc");
}
//实际关中断
static __
    always_inline void
    native_irq_disable(void)
{
    asm volatile("cli" ::
                     : "memory");
}
//实际开启中断
static __
    always_inline void
    native_irq_enable(void)
{
    asm volatile("sti" ::
                     : "memory");
}
// arch层关中断
static __
    always_inline void
    arch_local_irq_disable(void)
{
    native
        _
        irq_disable();
}
// arch层开启中断
static __
    always_inline void
    arch_local_irq_enable(void)
{
    native
        _
        irq_enable();
}
// arch层保存eflags寄存器
static __
    always_inline unsigned long
        arch
            _
                local
                    _
                        save
                            _
                            flags(void)
{
    return native
        _
            save
                _
                fl();
}
// arch层恢复eflags寄存器
static __
    always_inline void
    arch_local_irq_restore(unsigned long flags)
{
    native
        _
            restore
                _
                    fl(flags);
}
//实际保存eflags寄存器并关中断
static __
    always_inline unsigned long
    arch_local_irq_save(void)
{
    unsigned long flags = arch_local_save_flags();
    arch
        _
            local
                _
                irq_disable();
    return flags;
}
// raw层关闭开启中断宏
#define raw
_
    local
        _
        irq_disable() arch_local_irq_disable()
#define raw
            _
    local
    _
    irq_enable()
        arch
    _
    local
    _
    irq_enable()
// raw层保存恢复eflags寄存器宏
#define raw
        _
    local
    _
    irq_save(flags)

        do
{

    typecheck(unsigned long, flags);
    flags = arch_local_irq_save();
}
while (0)
#define raw
    _
        local
            _
            irq_restore(flags)

                do
    {

        typecheck(unsigned long, flags);
        arch
            _
                local
                    _
                        irq_restore(flags);
    }
while (0)
#define raw
    _
        local
            _
                save
                    _
                    flags(flags)

                        do
    {

        typecheck(unsigned long, flags);
        flags = arch_local_save_flags();
    }
while (0)
//通用层接口宏
#define local
    _ irq_enable()

        do
    {
        raw
            _
                local
                    _
                    irq_enable();
    }
while (0)
#define local
    _ irq_disable()

        do
    {

        raw
            _
                local
                    _
                    irq_disable();
    }
while (0)
#define local
    _ irq_save(flags)

        do
    {

        raw
            _
                local
                    _
                        irq_save(flags);
    }
while (0)
#define local
    _ irq_restore(flags)

        do
    {

        raw
            local
                irq_restore(flags);
    }

// spin lock
//最底层的自旋锁数据结构
typedef struct
{
    volatile unsigned long lock; //真正的锁值变量，用volatile标识
} spinlock_t;

#define spin_unlock_string \
                           \
    "
movb $1, % 0 " \ //写入1表示解锁
    : "
         = m
         "
         (lock->lock) : : "memory"
#define spin_lock_string \
                         \
    "
\n1 :\t " \
" lock;
decb % 0\n\t " \ //原子减1
             "
       js 2f\n " \ //当结果小于0则跳转到标号2处，表示加锁失败
               "
                   .section.text.lock,
\"ax\"\n" \ //重新定义一个代码段，这是优化技术，避免后面的代码
    "
    2 :\t " \
" cmpb $0,
    % 0\n\t " \ //和0比较
            "
        rep;
nop\n\t " \ //空指令
        "
    jle 2b\n\t " \ //小于或等于0跳转到标号2
               "
    jmp 1b\n " \ //跳转到标号1
             "
                 .previous "
    //获取自旋锁
    static inline void
    spin_lock(spinlock_t *lock)
{
    __ asm __ __ volatile __(
        spin_lock_string
        : "
          = m
          "
          (lock->lock)::"memory");
}
//释放自旋锁
static inline void spin_unlock(spinlock_t *lock)
{
    __ asm __ __ volatile __(
        spin_unlock_string);
}



// 信号量 作为资源计数器使用
struct semaphore
{
    raw
        _
            spinlock_t lock; //保护信号量自身的自旋锁
    unsigned int count;      //信号量值
    struct list
        _
            head wait
                _
                    list; //挂载睡眠等待进程的链表
};



// Linux 读写锁  读多写少场景
/* 共享 - 独占（shared-exclusive）锁， 当读写锁用读取模式加锁时，它是以
共享模式上锁的，当以写入修改模式加锁时，它是以独占模式上锁的（互斥）


*/
