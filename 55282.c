static int tower_release (struct inode *inode, struct file *file)
{
	struct lego_usb_tower *dev;
	int retval = 0;

	dev = file->private_data;

	if (dev == NULL) {
		retval = -ENODEV;
		goto exit_nolock;
	}

	mutex_lock(&open_disc_mutex);
	if (mutex_lock_interruptible(&dev->lock)) {
	        retval = -ERESTARTSYS;
		goto exit;
	}

	if (dev->open_count != 1) {
		dev_dbg(&dev->udev->dev, "%s: device not opened exactly once\n",
			__func__);
		retval = -ENODEV;
		goto unlock_exit;
	}
	if (dev->udev == NULL) {
		/* the device was unplugged before the file was released */

		/* unlock here as tower_delete frees dev */
		mutex_unlock(&dev->lock);
		tower_delete (dev);
		goto exit;
	}

	/* wait until write transfer is finished */
	if (dev->interrupt_out_busy) {
		wait_event_interruptible_timeout (dev->write_wait, !dev->interrupt_out_busy, 2 * HZ);
	}
	tower_abort_transfers (dev);
	dev->open_count = 0;

unlock_exit:
	mutex_unlock(&dev->lock);

exit:
	mutex_unlock(&open_disc_mutex);
exit_nolock:
	return retval;
}
