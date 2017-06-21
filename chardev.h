#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/types.h>
#include <asm/uaccess.h>
#include <linux/string.h>

#define MINOR_START 0
#define DEVICE_COUNT 2
#define BUFFER_SIZE 128

#ifndef DEF_RET_VAL_TIME
#define DEF_RET_VAL_TIME -1
#endif

#ifndef DEF_RET_VAL_NUMBER
#define DEF_RET_VAL_NUMBER -1
#endif

#define PDEBUG(fmt, args...) printk(KERN_DEBUG fmt, ## args)
#define MINIMUM(a,b)  ((a) < (b) ? (a) : (b))

