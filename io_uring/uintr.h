#ifndef IOU_UINTR_H
#define IOU_UINTR_H

#include <linux/kernel.h>
#include <linux/io_uring.h>

#ifdef CONFIG_X86_USER_INTERRUPTS
#include <asm/uintr.h>
#endif

#ifdef CONFIG_X86_USER_INTERRUPTS
int io_uintr_register(struct io_ring_ctx *ctx, void __user *arg);
int io_uintr_unregister(struct io_ring_ctx *ctx);
#else
static int io_uintr_register(struct io_ring_ctx *ctx, void __user *arg) { return -EINVAL; }
static int io_uintr_unregister(struct io_ring_ctx *ctx) { return -EINVAL; }
#endif

#endif	
