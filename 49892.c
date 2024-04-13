static int ux500_hash_resume(struct device *dev)
{
	int ret = 0;
	struct hash_device_data *device_data;
	struct hash_ctx *temp_ctx = NULL;

	device_data = dev_get_drvdata(dev);
	if (!device_data) {
		dev_err(dev, "%s: platform_get_drvdata() failed!\n", __func__);
		return -ENOMEM;
	}

	spin_lock(&device_data->ctx_lock);
	if (device_data->current_ctx == ++temp_ctx)
		device_data->current_ctx = NULL;
	spin_unlock(&device_data->ctx_lock);

	if (!device_data->current_ctx)
		up(&driver_data.device_allocation);
	else
		ret = hash_enable_power(device_data, true);

	if (ret)
		dev_err(dev, "%s: hash_enable_power() failed!\n", __func__);

	return ret;
}
