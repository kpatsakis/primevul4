static inline void iowarrior_delete(struct iowarrior *dev)
{
	dev_dbg(&dev->interface->dev, "minor %d\n", dev->minor);
	kfree(dev->int_in_buffer);
	usb_free_urb(dev->int_in_urb);
	kfree(dev->read_queue);
	kfree(dev);
}
