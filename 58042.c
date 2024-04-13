static void cp2112_remove(struct hid_device *hdev)
{
	struct cp2112_device *dev = hid_get_drvdata(hdev);
	int i;

	sysfs_remove_group(&hdev->dev.kobj, &cp2112_attr_group);
	i2c_del_adapter(&dev->adap);

	if (dev->gpio_poll) {
		dev->gpio_poll = false;
		cancel_delayed_work_sync(&dev->gpio_poll_worker);
	}

	for (i = 0; i < ARRAY_SIZE(dev->desc); i++) {
		gpiochip_unlock_as_irq(&dev->gc, i);
		gpiochip_free_own_desc(dev->desc[i]);
	}

	gpiochip_remove(&dev->gc);
	/* i2c_del_adapter has finished removing all i2c devices from our
	 * adapter. Well behaved devices should no longer call our cp2112_xfer
	 * and should have waited for any pending calls to finish. It has also
	 * waited for device_unregister(&adap->dev) to complete. Therefore we
	 * can safely free our struct cp2112_device.
	 */
	hid_hw_close(hdev);
	hid_hw_stop(hdev);
}
