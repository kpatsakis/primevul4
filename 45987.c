void comedi_error(const struct comedi_device *dev, const char *s)
{
	printk(KERN_ERR "comedi%d: %s: %s\n", dev->minor,
	       dev->driver->driver_name, s);
}
