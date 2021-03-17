/*
 * platform_device.h - generic, centralized driver model
 *
 * Copyright (c) 2001-2003 Patrick Mochel <mochel@osdl.org>
 *
 * This file is released under the GPLv2
 *
 * See Documentation/driver-model/ for more information.
 */

#ifndef _PLATFORM_DEVICE_H_
#define _PLATFORM_DEVICE_H_

#include "device.h"
//#include <linux/mod_devicetable.h>

#define PLATFORM_DEVID_NONE (-1)
#define PLATFORM_DEVID_AUTO (-2)

struct mfd_cell;

struct platform_device {
    const char  *init_name;
    int     id;
//  int     id_auto;
    struct  device  dev;
//  u32     num_resources;
//  struct resource *resource;
//  const struct platform_device_id *id_entry;
//  char *driver_override; /* Driver name to force a match */
//  /* MFD cell pointer */
//  struct mfd_cell *mfd_cell;
//  /* arch specific additions */
//  struct pdev_archdata    archdata;
};

#define platform_get_device_id(pdev)    ((pdev)->id_entry)

#define to_platform_device(x) container_of((x), struct platform_device, dev)

//extern int platform_device_register(struct platform_device *);
//extern void platform_device_unregister(struct platform_device *);

//extern struct bus_type platform_bus_type;
//extern struct device platform_bus;


struct platform_device_info {
    struct device *parent;
    const char *name;
    int id;
    unsigned int num_res;
    const void *data;
    int size_data;
    int dma_mask;
};

struct platform_driver {
    int (*probe)(struct platform_device *);
    struct device_driver driver;
//  const struct platform_device_id *id_table;
};


#define module_platform_driver_probe(__platform_driver, __platform_probe) \
static int __init __platform_driver##_init(void) \
{ \
    return platform_driver_probe(&(__platform_driver), \
                     __platform_probe);    \
} \
module_init(__platform_driver##_init); \
static void __exit __platform_driver##_exit(void) \
{ \
    platform_driver_unregister(&(__platform_driver)); \
} \
module_exit(__platform_driver##_exit);


extern int platform_driver_register(struct platform_driver *drv);
extern int platform_device_register(struct platform_device *pdev);


#endif /* _PLATFORM_DEVICE_H_ */
