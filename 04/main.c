#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/hid.h>
#include <linux/usb.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adam Taguirov <ataguiro@student.42.fr>");
MODULE_DESCRIPTION("Hello World module");

static int hid_hello(struct usb_interface *interface, const struct usb_device_id *id)
{
	printk(KERN_INFO "Hello, a keyboard is connected!\n");
	return 0;
}

static void hid_bye(struct usb_interface *interface)
{
	printk(KERN_INFO "Hello, a keyboard has been disconnected\n");
}

static struct usb_device_id hello_table [] = {
    { USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT, USB_INTERFACE_PROTOCOL_KEYBOARD) },
    { }                 /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, hello_table);

static struct usb_driver hid_driver = {
	.name = "hello_world",
	.id_table = hello_table,
	.probe = hid_hello,
	.disconnect = hid_bye,
};

static int __init hello_init(void) {
	int n;
	
	printk(KERN_INFO "Hello World !\n");
	n = usb_register(&hid_driver);
	return n;
}

static void __exit hello_cleanup(void) {
	printk(KERN_INFO "Cleaning up module.\n");
	usb_deregister(&hid_driver);
}

module_init(hello_init);
module_exit(hello_cleanup);
