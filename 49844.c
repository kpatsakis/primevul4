static int ux500_cryp_remove(struct platform_device *pdev)
{
	struct resource *res = NULL;
	struct resource *res_irq = NULL;
	struct cryp_device_data *device_data;

	dev_dbg(&pdev->dev, "[%s]", __func__);
	device_data = platform_get_drvdata(pdev);
	if (!device_data) {
		dev_err(&pdev->dev, "[%s]: platform_get_drvdata() failed!",
			__func__);
		return -ENOMEM;
	}

	/* Try to decrease the number of available devices. */
	if (down_trylock(&driver_data.device_allocation))
		return -EBUSY;

	/* Check that the device is free */
	spin_lock(&device_data->ctx_lock);
	/* current_ctx allocates a device, NULL = unallocated */
	if (device_data->current_ctx) {
		/* The device is busy */
		spin_unlock(&device_data->ctx_lock);
		/* Return the device to the pool. */
		up(&driver_data.device_allocation);
		return -EBUSY;
	}

	spin_unlock(&device_data->ctx_lock);

	/* Remove the device from the list */
	if (klist_node_attached(&device_data->list_node))
		klist_remove(&device_data->list_node);

	/* If this was the last device, remove the services */
	if (list_empty(&driver_data.device_list.k_list))
		cryp_algs_unregister_all();

	res_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res_irq)
		dev_err(&pdev->dev, "[%s]: IORESOURCE_IRQ, unavailable",
			__func__);
	else {
		disable_irq(res_irq->start);
		free_irq(res_irq->start, device_data);
	}

	if (cryp_disable_power(&pdev->dev, device_data, false))
		dev_err(&pdev->dev, "[%s]: cryp_disable_power() failed",
			__func__);

	clk_unprepare(device_data->clk);
	clk_put(device_data->clk);
	regulator_put(device_data->pwr_regulator);

	iounmap(device_data->base);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res)
		release_mem_region(res->start, resource_size(res));

	kfree(device_data);

	return 0;
}
