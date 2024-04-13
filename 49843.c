static int ux500_cryp_probe(struct platform_device *pdev)
{
	int ret;
	int cryp_error = 0;
	struct resource *res = NULL;
	struct resource *res_irq = NULL;
	struct cryp_device_data *device_data;
	struct cryp_protection_config prot = {
		.privilege_access = CRYP_STATE_ENABLE
	};
	struct device *dev = &pdev->dev;

	dev_dbg(dev, "[%s]", __func__);
	device_data = kzalloc(sizeof(struct cryp_device_data), GFP_ATOMIC);
	if (!device_data) {
		dev_err(dev, "[%s]: kzalloc() failed!", __func__);
		ret = -ENOMEM;
		goto out;
	}

	device_data->dev = dev;
	device_data->current_ctx = NULL;

	/* Grab the DMA configuration from platform data. */
	mem_to_engine = &((struct cryp_platform_data *)
			 dev->platform_data)->mem_to_engine;
	engine_to_mem = &((struct cryp_platform_data *)
			 dev->platform_data)->engine_to_mem;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "[%s]: platform_get_resource() failed",
				__func__);
		ret = -ENODEV;
		goto out_kfree;
	}

	res = request_mem_region(res->start, resource_size(res), pdev->name);
	if (res == NULL) {
		dev_err(dev, "[%s]: request_mem_region() failed",
				__func__);
		ret = -EBUSY;
		goto out_kfree;
	}

	device_data->phybase = res->start;
	device_data->base = ioremap(res->start, resource_size(res));
	if (!device_data->base) {
		dev_err(dev, "[%s]: ioremap failed!", __func__);
		ret = -ENOMEM;
		goto out_free_mem;
	}

	spin_lock_init(&device_data->ctx_lock);
	spin_lock_init(&device_data->power_state_spinlock);

	/* Enable power for CRYP hardware block */
	device_data->pwr_regulator = regulator_get(&pdev->dev, "v-ape");
	if (IS_ERR(device_data->pwr_regulator)) {
		dev_err(dev, "[%s]: could not get cryp regulator", __func__);
		ret = PTR_ERR(device_data->pwr_regulator);
		device_data->pwr_regulator = NULL;
		goto out_unmap;
	}

	/* Enable the clk for CRYP hardware block */
	device_data->clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(device_data->clk)) {
		dev_err(dev, "[%s]: clk_get() failed!", __func__);
		ret = PTR_ERR(device_data->clk);
		goto out_regulator;
	}

	ret = clk_prepare(device_data->clk);
	if (ret) {
		dev_err(dev, "[%s]: clk_prepare() failed!", __func__);
		goto out_clk;
	}

	/* Enable device power (and clock) */
	ret = cryp_enable_power(device_data->dev, device_data, false);
	if (ret) {
		dev_err(dev, "[%s]: cryp_enable_power() failed!", __func__);
		goto out_clk_unprepare;
	}

	cryp_error = cryp_check(device_data);
	if (cryp_error != 0) {
		dev_err(dev, "[%s]: cryp_init() failed!", __func__);
		ret = -EINVAL;
		goto out_power;
	}

	cryp_error = cryp_configure_protection(device_data, &prot);
	if (cryp_error != 0) {
		dev_err(dev, "[%s]: cryp_configure_protection() failed!",
			__func__);
		ret = -EINVAL;
		goto out_power;
	}

	res_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res_irq) {
		dev_err(dev, "[%s]: IORESOURCE_IRQ unavailable",
			__func__);
		ret = -ENODEV;
		goto out_power;
	}

	ret = request_irq(res_irq->start,
			  cryp_interrupt_handler,
			  0,
			  "cryp1",
			  device_data);
	if (ret) {
		dev_err(dev, "[%s]: Unable to request IRQ", __func__);
		goto out_power;
	}

	if (cryp_mode == CRYP_MODE_DMA)
		cryp_dma_setup_channel(device_data, dev);

	platform_set_drvdata(pdev, device_data);

	/* Put the new device into the device list... */
	klist_add_tail(&device_data->list_node, &driver_data.device_list);

	/* ... and signal that a new device is available. */
	up(&driver_data.device_allocation);

	atomic_set(&session_id, 1);

	ret = cryp_algs_register_all();
	if (ret) {
		dev_err(dev, "[%s]: cryp_algs_register_all() failed!",
			__func__);
		goto out_power;
	}

	dev_info(dev, "successfully registered\n");

	return 0;

out_power:
	cryp_disable_power(device_data->dev, device_data, false);

out_clk_unprepare:
	clk_unprepare(device_data->clk);

out_clk:
	clk_put(device_data->clk);

out_regulator:
	regulator_put(device_data->pwr_regulator);

out_unmap:
	iounmap(device_data->base);

out_free_mem:
	release_mem_region(res->start, resource_size(res));

out_kfree:
	kfree(device_data);
out:
	return ret;
}
