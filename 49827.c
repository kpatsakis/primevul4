static int cryp_enable_power(
		struct device *dev,
		struct cryp_device_data *device_data,
		bool restore_device_context)
{
	int ret = 0;

	dev_dbg(dev, "[%s]", __func__);

	spin_lock(&device_data->power_state_spinlock);
	if (!device_data->power_state) {
		ret = regulator_enable(device_data->pwr_regulator);
		if (ret) {
			dev_err(dev, "[%s]: regulator_enable() failed!",
					__func__);
			goto out;
		}

		ret = clk_enable(device_data->clk);
		if (ret) {
			dev_err(dev, "[%s]: clk_enable() failed!",
					__func__);
			regulator_disable(device_data->pwr_regulator);
			goto out;
		}
		device_data->power_state = true;
	}

	if (device_data->restore_dev_ctx) {
		spin_lock(&device_data->ctx_lock);
		if (restore_device_context && device_data->current_ctx) {
			device_data->restore_dev_ctx = false;
			cryp_restore_device_context(device_data,
					&device_data->current_ctx->dev_ctx);
		}
		spin_unlock(&device_data->ctx_lock);
	}
out:
	spin_unlock(&device_data->power_state_spinlock);

	return ret;
}
