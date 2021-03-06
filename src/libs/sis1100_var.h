/* $ZEL: sis1100_var.h,v 1.26 2010/01/18 19:04:14 wuestner Exp $ */

/*
 * Copyright (c) 2001-2009
 * 	Peter Wuestner.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _sis1100_var_h_
#define _sis1100_var_h_

#define SIS1100_MinVersion (13<<8)
#define SIS1100_MajVersion 2
#define SIS1100_Version (SIS1100_MinVersion|(SIS1100_MajVersion<<16))

#ifdef __NetBSD__
#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/proc.h>
#elif __linux__
#include "linux/ioctl.h"
#endif

/*
 * minorbits:
 * ccccttuu
 */

/* used bits of minor number */
#define sis1100_MINORBITS 8
/* bits used for cardnumber, number_of_cards<=(1<<MINORCARDBITS) */
#define sis1100_MINORCARDBITS 4
/* type 0: VME; type 1: SDRAM; type 2: sis1100_control; type 3: dsp*/
#define sis1100_MINORTYPEBITS 2

#define sis1100_MAXCARDS (1<<sis1100_MINORCARDBITS)
#define sis1100_MINORUSERBITS (sis1100_MINORBITS-sis1100_MINORCARDBITS-sis1100_MINORTYPEBITS)

#define sis1100_MINORCARDSHIFT (sis1100_MINORUSERBITS+sis1100_MINORTYPEBITS)
#define sis1100_MINORCARDMASK (((1<<sis1100_MINORCARDBITS)-1)<<(sis1100_MINORUSERBITS+sis1100_MINORTYPEBITS))
#define sis1100_MINORTYPESHIFT sis1100_MINORUSERBITS
#define sis1100_MINORTYPEMASK (((1<<sis1100_MINORTYPEBITS)-1)<<sis1100_MINORUSERBITS)
#define sis1100_MINORUSERSHIFT 0
#define sis1100_MINORUSERMASK ((1<<sis1100_MINORUSERBITS)-1)
#define sis1100_MINORUTMASK (sis1100_MINORTYPEMASK|sis1100_MINORUSERMASK)

enum sis1100_subdev {sis1100_subdev_remote, sis1100_subdev_ram,
    sis1100_subdev_ctrl, sis1100_subdev_dsp};
enum sis1100_hw_type {
    sis1100_hw_invalid=0,
    sis1100_hw_pci=1,
    sis1100_hw_vme=2,
    sis1100_hw_camac=3,
    sis1100_hw_lvd=4,
    sis1100_hw_pandapixel=5, /* FZJ only */
    sis1100_hw_psf4ad=6      /* Quad-14Bit-ADC */    
};

/*
 * error codes:
 * sis1100 local:
 *     0x005: deadlock (only transient)
 *     0x101: missing synchronisation
 *     0x102: inhibit
 *     0x103: output fifo full
 *     0x104: buffer full
 *     0x105: deadlock; transfer aborted
 *     0x107: timeout
 * sis1100 remote:
 *     0x202: not ready
 *     0x206: protocoll error
 *     0x207: timeout; indication not complete
 *     0x208: bus error
 *     0x209: fifo error
 * sis3100/sis3104 remote:
 *     0x211: VME bus error
 *     0x212: VME retry
 *     0x214: arbitration timeout
 *     0x219(?): bus error, generated by local crate controller because of timeout
 * sis5100 remote:
 *     0x2c0: CAMAC Q and X not set (Q=0x80, X=0x40)
 * lvd and pandapixel:
 *     same as sis1100 remote
 * PSF4AD remote:
 *     none
 * synthetic error codes:
 *     0x301: DMA interrupted
 *     0x302: synchronisation lost during DMA
 *     0x303 == 0x301|0x302
 */

struct sis1100_vme_req {
    int size;
    int32_t am;
    u_int32_t addr;
    u_int32_t data;
    u_int32_t error;
};

/*
 * struct sis1100_vme_req {
 *     int size;
 *     int32_t am;
 *     u_int32_t addr;
 *     union {
 *         u_int8_t u8;
 *         u_int16_t u16;
 *         u_int32_t u32;
 *         u_int64_t u64;
 *     } data;
 *     u_int32_t error;
 * };
 */

struct sis1100_vme_block_req {
    int size;        /* size of dataword */
    int fifo;
    size_t num;      /* number of datawords */
    int32_t am;
    u_int32_t addr;  /* remote bus address */
    u_int8_t* data;  /* local user space address */
    u_int32_t error;
};

struct sis1100_vme_super_block_req {
    int n;
    int error;
    struct sis1100_vme_block_req* reqs;
};

struct sis1100_camac_req {
    u_int32_t N;
    u_int32_t A;
    u_int32_t F;
    u_int32_t data;
    u_int32_t error;
};
#if 0
struct sis1100_camac_scan_req {
    u_int32_t N;
    u_int32_t A;
    u_int32_t F;
    u_int32_t data;
    u_int32_t error;
};
#endif
struct sis1100_pipelist {
    u_int32_t head; /* masked with 0xff3f0400:                  */
    	    	    /* only 'be', remote space and w/r are used */
    int32_t am;
    u_int32_t addr;
    u_int32_t data; /* only used for write access */
};

struct sis1100_pipe {
    int num;
    struct sis1100_pipelist* list;
    u_int32_t* data;
    u_int32_t error;
};

struct sis1100_writepipe {
    int num;
    int am;
    u_int32_t* data; /* num*{addr, data} */
    u_int32_t error;
};

struct vmespace {
    int32_t am;
    u_int32_t datasize;
    int swap;          /* 1: swap words 0: don't swap -1: not changed */
    int mapit;         /* not used */
    ssize_t mindmalen; /* 0: never use DMA; 1: always use DMA; -1: not changed */
};

struct sis1100_ident_dev {
    enum sis1100_hw_type hw_type;
    int hw_version;
    int fw_type;
    int fw_version;
};

struct sis1100_ident {
    struct sis1100_ident_dev local;
    struct sis1100_ident_dev remote;
    int remote_ok;
    int remote_online;
};

struct sis1100_ctrl_reg {
    int offset;
    u_int32_t val;
    u_int32_t error;
};

struct sis1100_ctrl_rw {
    u_int32_t offset;
    u_int32_t fifo;
    u_int32_t count;
    u_int32_t *data;
    u_int32_t error;
};

struct sis1100_irq_ctl {
    u_int32_t irq_mask;
    int32_t   signal;  /* >0: signal; ==0: disable; <0: no signal but select */
};

struct sis1100_irq_ctl2 {
    u_int32_t irq_mask;
    int32_t   signal;  /* >0: signal; ==0: disable; <0: no signal but select */
    /* the fields above this line have to match sis1100_irq_ctl */
    u_int32_t auto_mask; /* these IRQs are automatically reenabled */
    u_int32_t flags;     /* for future use, should be zero */
};

struct sis1100_irq_get {
    u_int32_t irq_mask;      /* IRQs for which information is requested */
    int32_t   remote_status; /* -1: down 1: up 0: unchanged */
    u_int32_t opt_status;    /* content of opt. status register of sis1100 */
    u_int32_t mbx0;          /* content of mailbox 0 of PLX */
    u_int32_t irqs;          /* pending IRQs */
    int32_t   level;         /* highest level of pendig VME IRQs */
    int32_t   vector;        /* corresponding IRQ vector */
};

struct sis1100_irq_get2 {
    u_int32_t irq_mask;
    int32_t   remote_status; /* -1: down 1: up 0: unchanged */
    u_int32_t opt_status;
    u_int32_t mbx0;
    u_int32_t irqs;
    int32_t level;
    int32_t vector;
    /* the fields above this line have to match sis1100_irq_get */
    u_int32_t irq_sec;  /* time of last Interrupt (seconds)     */
    u_int32_t irq_nsec; /*                        (nanoseconds) */
};

struct sis1100_irq_ack {
    u_int32_t irq_mask;
};

struct sis1100_dma_alloc {
    size_t size;
    off_t offset;
    u_int32_t dma_addr;
};

struct sis1100_dsp_code {
    void*     src;  /* pointer to code */
    u_int32_t dst;  /* load address in SHARC memory*/
    int       size; /* code size in bytes */
};

struct sis1100_eeprom_req {
    u_int8_t num;    /* number of 16-bit-words */
    u_int8_t addr;   /* eeprom address */
    u_int16_t* data; /* user space address */
};

struct sis1100_ddma_map {
    char* addr;      /* virtual address (page aligned) */
    size_t size;     /* size of ONE block (multiple of page size) */
    int num;         /* number of blocks (>=2) */
};

struct sis1100_ddma_stop {
    u_int64_t num; /* all! transferred bytes */
    int idx;       /* index of last block written */
};

#define SIS3100_VME_IRQS      0xFE
#define SIS3100_FLAT_IRQS    0xF00
#define SIS3100_LEMO_IRQS   0x7000
#define SIS3100_DSP_IRQ     0x8000
#define SIS3100_FRONT_IRQS (SIS3100_FLAT_IRQS  | SIS3100_LEMO_IRQS)
#define SIS3100_EXT_IRQS   (SIS3100_FRONT_IRQS | SIS3100_DSP_IRQ  )
#define SIS3100_IRQS       (SIS3100_VME_IRQS   | SIS3100_EXT_IRQS )

#define SIS5100_LAM_IRQS   0x007fffff
#define SIS5100_FLAT_IRQS  0x07000000
#define SIS5100_LEMO_IRQS  0x70000000
#define SIS5100_DSP_IRQ    0x80000000
#define SIS5100_FRONT_IRQS (SIS5100_FLAT_IRQS  | SIS5100_LEMO_IRQS)
#define SIS5100_EXT_IRQS   (SIS5100_FRONT_IRQS | SIS5100_DSP_IRQ  )
#define SIS5100_IRQS       (SIS5100_LAM_IRQS   | SIS5100_EXT_IRQS )  

#define SIS1100_FRONT_IRQS  0x30000 /* only if remote side==SIS310x */
#define SIS1100_MBX0_IRQ   0x100000 /* only if remote side==SIS310x */
#define SIS1100_IRQS       (SIS1100_FRONT_IRQS|SIS1100_MBX0_IRQ)

#define ZELLVD_DDMA_IRQ (1<<30)
#define ZELLVD_DDMA_IRQS ZELLVD_DDMA_IRQ

#define PANDAPIXEL_EV_AV 0x00000001

#define GLINK_MAGIC 'g'

#define SIS1100_NEW_CTRL

#define SIS1100_SETVMESPACE     _IOW (GLINK_MAGIC,  1, struct vmespace)
#define SIS3100_VME_PROBE       _IOW (GLINK_MAGIC,  2, u_int32_t)
#define SIS3100_VME_READ        _IOWR(GLINK_MAGIC,  3, struct sis1100_vme_req)
#define SIS3100_VME_WRITE       _IOWR(GLINK_MAGIC,  4, struct sis1100_vme_req)
#define SIS3100_VME_BLOCK_READ  _IOWR(GLINK_MAGIC,  5, struct sis1100_vme_block_req)
#define SIS3100_VME_BLOCK_WRITE _IOWR(GLINK_MAGIC,  6, struct sis1100_vme_block_req)
#ifdef SIS1100_NEW_CTRL
#define SIS1100_CTRL_READ       _IOWR(GLINK_MAGIC,  7, struct sis1100_ctrl_reg)
#define SIS1100_CTRL_WRITE      _IOWR(GLINK_MAGIC,  8, struct sis1100_ctrl_reg)
#else
#define SIS1100_LOCAL_CTRL_READ _IOWR(GLINK_MAGIC,  7, struct sis1100_ctrl_reg)
#define SIS1100_LOCAL_CTRL_WRITE _IOWR(GLINK_MAGIC, 8, struct sis1100_ctrl_reg)
#define SIS1100_REMOTE_CTRL_READ _IOWR(GLINK_MAGIC, 9, struct sis1100_ctrl_reg)
#define SIS1100_REMOTE_CTRL_WRITE _IOWR(GLINK_MAGIC, 10, struct sis1100_ctrl_reg)
#endif
#define SIS1100_PIPE            _IOWR(GLINK_MAGIC, 11, struct sis1100_pipe)
#define SIS1100_MAPSIZE         _IOR (GLINK_MAGIC, 12, u_int32_t)
#define SIS1100_LAST_ERROR      _IOR (GLINK_MAGIC, 13, u_int32_t)
#define SIS1100_IDENT           _IOR (GLINK_MAGIC, 14, struct sis1100_ident)
#define SIS1100_FIFOMODE        _IOWR(GLINK_MAGIC, 15, int)

#define SIS1100_IRQ_CTL         _IOW (GLINK_MAGIC, 17, struct sis1100_irq_ctl)
#define SIS1100_IRQ_CTL2        _IOW (GLINK_MAGIC, 52, struct sis1100_irq_ctl2)
#define SIS1100_IRQ_GET         _IOWR(GLINK_MAGIC, 18, struct sis1100_irq_get)
#define SIS1100_IRQ_GET2        _IOWR(GLINK_MAGIC, 53, struct sis1100_irq_get2)
#define SIS1100_IRQ_ACK         _IOW (GLINK_MAGIC, 19, struct sis1100_irq_ack)
#define SIS1100_IRQ_WAIT        _IOWR(GLINK_MAGIC, 20, struct sis1100_irq_get)
#define SIS1100_IRQ_WAIT2       _IOWR(GLINK_MAGIC, 54, struct sis1100_irq_get2)

#define SIS1100_MINDMALEN       _IOWR(GLINK_MAGIC, 21, int[2])

#define SIS1100_FRONT_IO        _IOWR(GLINK_MAGIC, 22, u_int32_t)
#define SIS1100_FRONT_PULSE     _IOW (GLINK_MAGIC, 23, u_int32_t)
#define SIS1100_FRONT_LATCH     _IOWR(GLINK_MAGIC, 24, u_int32_t)

#define SIS3100_VME_SUPER_BLOCK_READ _IOWR(GLINK_MAGIC, 25, struct sis1100_vme_super_block_req)
#define SIS1100_WRITE_PIPE      _IOWR(GLINK_MAGIC, 26, struct sis1100_writepipe)

#define SIS1100_DMA_ALLOC       _IOWR(GLINK_MAGIC, 27, struct sis1100_dma_alloc)
#define SIS1100_DMA_FREE        _IOW (GLINK_MAGIC, 28, struct sis1100_dma_alloc)

#define SIS5100_CCCZ            _IO  (GLINK_MAGIC, 29)
#define SIS5100_CCCC            _IO  (GLINK_MAGIC, 30)
#define SIS5100_CCCI            _IOW (GLINK_MAGIC, 31, int)
#define SIS5100_CNAF            _IOWR(GLINK_MAGIC, 32, struct sis1100_camac_req)
#define SIS1100_SWAP            _IOWR(GLINK_MAGIC, 33, int)
#define SIS3100_TIMEOUTS        _IOWR(GLINK_MAGIC, 34, int[2])

#define SIS1100_DSP_LOAD        _IOW (GLINK_MAGIC, 35, struct sis1100_dsp_code)
#define SIS1100_DSP_RESET       _IO  (GLINK_MAGIC, 36)
#define SIS1100_DSP_START       _IO  (GLINK_MAGIC, 37)

/*#define SIS1100_CARD_IDX        _IOR (GLINK_MAGIC, 38, int)*/

#define SIS1100_DEMAND_DMA_MAP   _IOW (GLINK_MAGIC, 39, struct sis1100_ddma_map)
#define SIS1100_DEMAND_DMA_START _IO  (GLINK_MAGIC, 40)
#define SIS1100_DEMAND_DMA_STOP  _IOR (GLINK_MAGIC, 41, struct sis1100_ddma_stop)
#define SIS1100_DEMAND_DMA_MARK  _IOW (GLINK_MAGIC, 42, int)
#define SIS1100_DEMAND_DMA_WAIT  _IOWR(GLINK_MAGIC, 43, int)
#define SIS3100_VME_WRITE_BLIND  _IOWR(GLINK_MAGIC, 44, struct sis1100_vme_req)

#define SIS1100_SERIAL_NO        _IOR (GLINK_MAGIC, 45, u_int32_t[4])
#define SIS1100_DSP_READ         _IOWR(GLINK_MAGIC, 46, struct sis1100_dsp_code)

#define SIS1100_MINPIPELEN       _IOWR(GLINK_MAGIC, 47, int[2])

#define SIS1100_DSP_WR           _IOW (GLINK_MAGIC, 50, struct sis1100_dsp_code)
#define SIS1100_DSP_RD           _IOWR(GLINK_MAGIC, 51, struct sis1100_dsp_code)
/* 52: SIS1100_IRQ_CTL2 */
/* 53: SIS1100_IRQ_GET2 */
/* 54: SIS1100_IRQ_WAIT2 */
#define SIS1100_CTRL_READ_BLOCK  _IOWR(GLINK_MAGIC, 55, struct sis1100_ctrl_rw)
#define SIS1100_CTRL_WRITE_BLOCK _IOWR(GLINK_MAGIC, 56, struct sis1100_ctrl_rw)

/* the following functions (1xx) are not designed for "daily use",
   but will be usefull for testing and debugging */
#define SIS1100_RESET           _IO  (GLINK_MAGIC, 102)
#define SIS1100_REMOTE_RESET    _IO  (GLINK_MAGIC, 103)
#define SIS1100_DEVTYPE         _IOR (GLINK_MAGIC, 104, enum sis1100_subdev)
#define SIS1100_DRIVERVERSION   _IOR (GLINK_MAGIC, 105, int)
#define SIS1100_READ_EEPROM     _IOW (GLINK_MAGIC, 106, struct sis1100_eeprom_req)
#define SIS1100_WRITE_EEPROM    _IOW (GLINK_MAGIC, 107, struct sis1100_eeprom_req)
#define SIS1100_JTAG_ENABLE     _IOW (GLINK_MAGIC, 108, u_int32_t)
#define SIS1100_JTAG_CTRL       _IOWR(GLINK_MAGIC, 109, u_int32_t)
#define SIS1100_JTAG_DATA       _IOR (GLINK_MAGIC, 110, u_int32_t)
#define SIS1100_JTAG_PUT        _IOW (GLINK_MAGIC, 111, u_int32_t)
#define SIS1100_JTAG_GET        _IOR (GLINK_MAGIC, 112, u_int32_t)
#define SIS1100_PLX_READ        _IOWR(GLINK_MAGIC, 113, struct sis1100_ctrl_reg)
#define SIS1100_PLX_WRITE       _IOWR(GLINK_MAGIC, 114, struct sis1100_ctrl_reg)
#define SIS1100_EEPROM_SIZE     _IOR (GLINK_MAGIC, 115, int)
#define SIS1100_TESTFLAGS       _IOWR(GLINK_MAGIC, 120, u_int32_t)
#define SIS1100_TRANSPARENT     _IOWR(GLINK_MAGIC, 130, int32_t)

#ifndef PURE_SIS1100_NAMESPACE
#define SETVMESPACE SIS1100_SETVMESPACE
#define VME_PROBE SIS3100_VME_PROBE
#endif

#endif
