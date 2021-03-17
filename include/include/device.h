/*
 * device.h - generic, centralized driver model
 *
 * Copyright (c) 2001-2003 Patrick Mochel <mochel@osdl.org>
 * Copyright (c) 2004-2009 Greg Kroah-Hartman <gregkh@suse.de>
 * Copyright (c) 2008-2009 Novell Inc.
 *
 * This file is released under the GPLv2
 *
 * See Documentation/driver-model/ for more information.
 */

#ifndef _DEVICE_H_
#define _DEVICE_H_

//#include <linux/ioport.h>
//#include <linux/kobject.h>
//#include <linux/klist.h>
//#include <linux/list.h>
//#include <linux/lockdep.h>
//#include <linux/compiler.h>
//#include <linux/types.h>
//#include <linux/mutex.h>
//#include <linux/pinctrl/devinfo.h>
//#include <linux/pm.h>
//#include <linux/atomic.h>
//#include <linux/ratelimit.h>
//#include <linux/uidgid.h>
//#include <linux/gfp.h>
//#include <asm/device.h>


/**
 * struct bus_type - The bus type of the device
 *
 * @name:   The name of the bus.
 * @dev_name:   Used for subsystems to enumerate devices like ("foo%u", dev->id).
 * @dev_root:   Default device to use as the parent.
 * @dev_attrs:  Default attributes of the devices on the bus.
 * @bus_groups: Default attributes of the bus.
 * @dev_groups: Default attributes of the devices on the bus.
 * @drv_groups: Default attributes of the device drivers on the bus.
 * @match:  Called, perhaps multiple times, whenever a new device or driver
 *      is added for this bus. It should return a nonzero value if the
 *      given device can be handled by the given driver.
 * @uevent: Called when a device is added, removed, or a few other things
 *      that generate uevents to add the environment variables.
 * @probe:  Called when a new device or driver add to this bus, and callback
 *      the specific driver's probe to initial the matched device.
 * @remove: Called when a device removed from this bus.
 * @shutdown:   Called at shut-down time to quiesce the device.
 *
 * @online: Called to put the device back online (after offlining it).
 * @offline:    Called to put the device offline for hot-removal. May fail.
 *
 * @suspend:    Called when a device on this bus wants to go to sleep mode.
 * @resume: Called to bring a device on this bus out of sleep mode.
 * @pm:     Power management operations of this bus, callback the specific
 *      device driver's pm-ops.
 * @iommu_ops:  IOMMU specific operations for this bus, used to attach IOMMU
 *              driver implementations to a bus and allow the driver to do
 *              bus-specific setup
 * @p:      The private data of the driver core, only the driver core can
 *      touch this.
 * @lock_key:   Lock class key for use by the lock validator
 *
 * A bus is a channel between the processor and one or more devices. For the
 * purposes of the device model, all devices are connected via a bus, even if
 * it is an internal, virtual, "platform" bus. Buses can plug into each other.
 * A USB controller is usually a PCI device, for example. The device model
 * represents the actual connections between buses and the devices they control.
 * A bus is represented by the bus_type structure. It contains the name, the
 * default attributes, the bus' methods, PM operations, and the driver core's
 * private data.
 */

struct device {
    struct device    *parent;
    struct bus_type  *bus;
    const char       *name;   /* initial name of the device */
    struct device_driver *driver; /* which driver has allocated this device */
    void        *platform_data;   /* Platform specific data, devicecore doesn't touch it */
    void        *driver_data;     /* Driver data, set and get with*/
};

struct bus_type {
    const char      *name;
    const char      *dev_name;
    struct device   *dev_root;
    int (*match)(struct device *dev, struct device_driver *drv);
    int (*probe)(struct device *dev);
};


struct device_driver {
    const char       *name;
    struct bus_type  *bus;
    const struct of_device_id   *of_match_table;
    int (*probe) (struct device *dev);
};

struct device_type {
    const char *name;
    const struct attribute_group **groups;
};

#define module_driver(__driver, __register, __unregister, ...) \
static int __init __driver##_init(void) \
{ \
    return __register(&(__driver) , ##__VA_ARGS__); \
} \
module_init(__driver##_init); \
static void __exit __driver##_exit(void) \
{ \
    __unregister(&(__driver) , ##__VA_ARGS__); \
} \
module_exit(__driver##_exit);

#endif /* _DEVICE_H_ */
