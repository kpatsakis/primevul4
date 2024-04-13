static void ux500_cryp_shutdown(struct platform_device *pdev)
{
	struct resource *res_irq = NULL;
	struct cryp_device_data *device_data;

	dev_dbg(&pdev->dev, "[%s]", __func__);

	device_data = platform_get_drvdata(pdev);
	if (!device_data) {
		dev_err(&pdev->dev, "[%s]: platform_get_drvdata() failed!",
			__func__);
		return;
	}

	/* Check that the device is free */
	spin_lock(&device_data->ctx_lock);
	/* current_ctx allocates a device, NULL = unallocated */
	if (!device_data->current_ctx) {
		if (down_trylock(&driver_data.device_allocation))
			dev_dbg(&pdev->dev, "[%s]: Cryp still in use!"
				"Shutting down anyway...", __func__);
		/**
		 * (Allocate the device)
		 * Need to set this to non-null (dummy) value,
		 * to avoid usage if context switching.
		 */
		device_data->current_ctx++;
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

}
