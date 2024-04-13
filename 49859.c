static int hash_disable_power(struct hash_device_data *device_data,
			      bool save_device_state)
{
	int ret = 0;
	struct device *dev = device_data->dev;

	spin_lock(&device_data->power_state_lock);
	if (!device_data->power_state)
		goto out;

	if (save_device_state) {
		hash_save_state(device_data,
				&device_data->state);
		device_data->restore_dev_state = true;
	}

	clk_disable(device_data->clk);
	ret = regulator_disable(device_data->regulator);
	if (ret)
		dev_err(dev, "%s: regulator_disable() failed!\n", __func__);

	device_data->power_state = false;

out:
	spin_unlock(&device_data->power_state_lock);

	return ret;
}
