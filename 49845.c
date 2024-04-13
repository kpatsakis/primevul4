static int ux500_cryp_resume(struct device *dev)
{
	int ret = 0;
	struct platform_device *pdev = to_platform_device(dev);
	struct cryp_device_data *device_data;
	struct resource *res_irq;
	struct cryp_ctx *temp_ctx = NULL;

	dev_dbg(dev, "[%s]", __func__);

	device_data = platform_get_drvdata(pdev);
	if (!device_data) {
		dev_err(dev, "[%s]: platform_get_drvdata() failed!", __func__);
		return -ENOMEM;
	}

	spin_lock(&device_data->ctx_lock);
	if (device_data->current_ctx == ++temp_ctx)
		device_data->current_ctx = NULL;
	spin_unlock(&device_data->ctx_lock);


	if (!device_data->current_ctx)
		up(&driver_data.device_allocation);
	else
		ret = cryp_enable_power(dev, device_data, true);

	if (ret)
		dev_err(dev, "[%s]: cryp_enable_power() failed!", __func__);
	else {
		res_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
		if (res_irq)
			enable_irq(res_irq->start);
	}

	return ret;
}
