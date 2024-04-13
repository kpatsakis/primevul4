static int cryp_disable_power(struct device *dev,
			      struct cryp_device_data *device_data,
			      bool save_device_context)
{
	int ret = 0;

	dev_dbg(dev, "[%s]", __func__);

	spin_lock(&device_data->power_state_spinlock);
	if (!device_data->power_state)
		goto out;

	spin_lock(&device_data->ctx_lock);
	if (save_device_context && device_data->current_ctx) {
		cryp_save_device_context(device_data,
				&device_data->current_ctx->dev_ctx,
				cryp_mode);
		device_data->restore_dev_ctx = true;
	}
	spin_unlock(&device_data->ctx_lock);

	clk_disable(device_data->clk);
	ret = regulator_disable(device_data->pwr_regulator);
	if (ret)
		dev_err(dev, "[%s]: "
				"regulator_disable() failed!",
				__func__);

	device_data->power_state = false;

out:
	spin_unlock(&device_data->power_state_spinlock);

	return ret;
}
