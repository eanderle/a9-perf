#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>

#define DRIVER_AUTHOR "Eric Anderle"
#define DRIVER_DESC   "ARM Cortex A9 Performance Counters"
#define DRIVER_VER    "0.1"

#define RENAME_INST_EVENT 0x68

long long accumulated_inst_count = 0;

static inline void select_idx(int idx) {
    asm volatile("mcr p15, 0, %0, c9, c12, 5" : : "r" (idx));
    asm volatile("isb");
}

static inline void enable_event_for_idx(int event, int idx) {
    select_idx(idx);
    // Select event type
    asm volatile("mcr p15, 0, %0, c9, c13, 1" : : "r" (event));
    // Count enable
    asm volatile("mcr p15, 0, %0, c9, c12, 1" : : "r" (0x1));
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
    // Enable and clear all counters
    asm volatile("mcr p15, 0, %0, c9, c12, 0" : : "r" (0x3));
    // Clear overflow bits
    asm volatile("mcr p15, 0, %0, c9, c12, 3" : : "r" (0x0));

    enable_event_for_idx(RENAME_INST_EVENT, 0);

    return 0;
}

static void __exit a9_perf_end(void)
{
    printk("Instructions out of core renaming stage: %lld\n", accumulated_inst_count);
    printk("Cortex A9 Performance Counters version %s unloaded\n", DRIVER_VER);
}

module_init(a9_perf_start);
module_exit(a9_perf_end);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
