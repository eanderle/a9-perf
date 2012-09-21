#include <asm/cacheflush.h>
#include <asm/io.h>
#include <asm/memory.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#define DRIVER_AUTHOR "Eric Anderle"
#define DRIVER_DESC   "ARM Cortex A9 Performance Counters"
#define DRIVER_VER    "0.1"

static inline void select_idx(int idx) {
    asm volatile("mcr p15, 0, %0, c9, c12, 5" : : "r" (idx));
}

static inline void enable_event_for_idx(int event, int idx) {
    select_idx(idx);
    asm volatile("mcr p15, 0, %0, c9, c13, 1" : : "r" (event));
}

static inline long long get_count_for_idx(int idx) {
    long long count;
    select_idx(idx);
    asm volatile("mrc p15, 0, %0, c9, c13, 2" : "=r" (count));
    return count;
}

static int __init a9_perf_start(void)
{
    printk("Cortex A9 Performance Counters version %s loaded\n", DRIVER_VER);
    enable_event_for_idx(0x68, 0);
    return 0;
}

static void __exit a9_perf_end(void)
{
    printk("Instructions out of core renaming stage: ", get_count_for_idx(0));
    printk("Cortex A9 Performance Counters version %s unloaded\n", DRIVER_VER);
}

module_init(a9_perf_start);
module_exit(a9_perf_end);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
