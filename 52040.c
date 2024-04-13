void usb_devio_cleanup(void)
{
	usb_unregister_notify(&usbdev_nb);
	cdev_del(&usb_device_cdev);
	unregister_chrdev_region(USB_DEVICE_DEV, USB_DEVICE_MAX);
}
