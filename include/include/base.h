extern int bus_for_each_dev(struct bus_type *bus, struct device *start, void *data, int (*fn)(struct device *, void *));
extern int driver_register(struct device_driver *drv);
extern int bus_add_driver(struct device_driver *drv);
extern int bus_add_device(struct device *dev);
