https://www.kernel.org/doc/Documentation/gpio/sysfs.txt

This is the OLD way of triggering a GPIO with sysfs.

it is still available on yocto for imx8 but it should be avoided.

==================================
GPIO Sysfs Interface for Userspace
==================================

THIS ABI IS DEPRECATED, THE ABI DOCUMENTATION HAS BEEN MOVED TO
Documentation/ABI/obsolete/sysfs-gpio AND NEW USERSPACE CONSUMERS
ARE SUPPOSED TO USE THE CHARACTER DEVICE ABI. THIS OLD SYSFS ABI WILL
NOT BE DEVELOPED (NO NEW FEATURES), IT WILL JUST BE MAINTAINED.

Refer to the examples in tools/gpio/* for an introduction to the new
character device ABI. Also see the userspace header in
include/uapi/linux/gpio.h
