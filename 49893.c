static int ux500_hash_suspend(struct device *dev)
{
	int ret;
	struct hash_device_data *device_data;
	struct hash_ctx *temp_ctx = NULL;

	device_data = dev_get_drvdata(dev);
	if (!device_data) {
		dev_err(dev, "%s: platform_get_drvdata() failed!\n", __func__);
		return -ENOMEM;
	}

	spin_lock(&device_data->ctx_lock);
	if (!device_data->current_ctx)
		device_data->current_ctx++;
	spin_unlock(&device_data->ctx_lock);

	if (device_data->current_ctx == ++temp_ctx) {
		if (down_interruptible(&driver_data.device_allocation))
			dev_dbg(dev, "%s: down_interruptible() failed\n",
				__func__);
		ret = hash_disable_power(device_data, false);

	} else {
		ret = hash_disable_power(device_data, true);
	}

	if (ret)
		dev_err(dev, "%s: hash_disable_power()\n", __func__);

	return ret;
}
