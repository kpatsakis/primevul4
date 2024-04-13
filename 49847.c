static int ux500_cryp_suspend(struct device *dev)
{
	int ret;
	struct platform_device *pdev = to_platform_device(dev);
	struct cryp_device_data *device_data;
	struct resource *res_irq;
	struct cryp_ctx *temp_ctx = NULL;

	dev_dbg(dev, "[%s]", __func__);

	/* Handle state? */
	device_data = platform_get_drvdata(pdev);
	if (!device_data) {
		dev_err(dev, "[%s]: platform_get_drvdata() failed!", __func__);
		return -ENOMEM;
	}

	res_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res_irq)
		dev_err(dev, "[%s]: IORESOURCE_IRQ, unavailable", __func__);
	else
		disable_irq(res_irq->start);

	spin_lock(&device_data->ctx_lock);
	if (!device_data->current_ctx)
		device_data->current_ctx++;
	spin_unlock(&device_data->ctx_lock);

	if (device_data->current_ctx == ++temp_ctx) {
		if (down_interruptible(&driver_data.device_allocation))
			dev_dbg(dev, "[%s]: down_interruptible() failed",
				__func__);
		ret = cryp_disable_power(dev, device_data, false);

	} else
		ret = cryp_disable_power(dev, device_data, true);

	if (ret)
		dev_err(dev, "[%s]: cryp_disable_power()", __func__);

	return ret;
}
