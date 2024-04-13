static int hash_enable_power(struct hash_device_data *device_data,
			     bool restore_device_state)
{
	int ret = 0;
	struct device *dev = device_data->dev;

	spin_lock(&device_data->power_state_lock);
	if (!device_data->power_state) {
		ret = regulator_enable(device_data->regulator);
		if (ret) {
			dev_err(dev, "%s: regulator_enable() failed!\n",
				__func__);
			goto out;
		}
		ret = clk_enable(device_data->clk);
		if (ret) {
			dev_err(dev, "%s: clk_enable() failed!\n", __func__);
			ret = regulator_disable(
					device_data->regulator);
			goto out;
		}
		device_data->power_state = true;
	}

	if (device_data->restore_dev_state) {
		if (restore_device_state) {
			device_data->restore_dev_state = false;
			hash_resume_state(device_data, &device_data->state);
		}
	}
out:
	spin_unlock(&device_data->power_state_lock);

	return ret;
}
