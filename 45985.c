static void comedi_device_cleanup(struct comedi_device *dev)
{
	if (dev == NULL)
		return;
	mutex_lock(&dev->mutex);
	comedi_device_detach(dev);
	mutex_unlock(&dev->mutex);
	mutex_destroy(&dev->mutex);
}
