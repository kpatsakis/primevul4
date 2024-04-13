static int ux500_hash_probe(struct platform_device *pdev)
{
	int			ret = 0;
	struct resource		*res = NULL;
	struct hash_device_data *device_data;
	struct device		*dev = &pdev->dev;

	device_data = kzalloc(sizeof(*device_data), GFP_ATOMIC);
	if (!device_data) {
		ret = -ENOMEM;
		goto out;
	}

	device_data->dev = dev;
	device_data->current_ctx = NULL;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_dbg(dev, "%s: platform_get_resource() failed!\n", __func__);
		ret = -ENODEV;
		goto out_kfree;
	}

	res = request_mem_region(res->start, resource_size(res), pdev->name);
	if (res == NULL) {
		dev_dbg(dev, "%s: request_mem_region() failed!\n", __func__);
		ret = -EBUSY;
		goto out_kfree;
	}

	device_data->phybase = res->start;
	device_data->base = ioremap(res->start, resource_size(res));
	if (!device_data->base) {
		dev_err(dev, "%s: ioremap() failed!\n", __func__);
		ret = -ENOMEM;
		goto out_free_mem;
	}
	spin_lock_init(&device_data->ctx_lock);
	spin_lock_init(&device_data->power_state_lock);

	/* Enable power for HASH1 hardware block */
	device_data->regulator = regulator_get(dev, "v-ape");
	if (IS_ERR(device_data->regulator)) {
		dev_err(dev, "%s: regulator_get() failed!\n", __func__);
		ret = PTR_ERR(device_data->regulator);
		device_data->regulator = NULL;
		goto out_unmap;
	}

	/* Enable the clock for HASH1 hardware block */
	device_data->clk = clk_get(dev, NULL);
	if (IS_ERR(device_data->clk)) {
		dev_err(dev, "%s: clk_get() failed!\n", __func__);
		ret = PTR_ERR(device_data->clk);
		goto out_regulator;
	}

	ret = clk_prepare(device_data->clk);
	if (ret) {
		dev_err(dev, "%s: clk_prepare() failed!\n", __func__);
		goto out_clk;
	}

	/* Enable device power (and clock) */
	ret = hash_enable_power(device_data, false);
	if (ret) {
		dev_err(dev, "%s: hash_enable_power() failed!\n", __func__);
		goto out_clk_unprepare;
	}

	ret = hash_check_hw(device_data);
	if (ret) {
		dev_err(dev, "%s: hash_check_hw() failed!\n", __func__);
		goto out_power;
	}

	if (hash_mode == HASH_MODE_DMA)
		hash_dma_setup_channel(device_data, dev);

	platform_set_drvdata(pdev, device_data);

	/* Put the new device into the device list... */
	klist_add_tail(&device_data->list_node, &driver_data.device_list);
	/* ... and signal that a new device is available. */
	up(&driver_data.device_allocation);

	ret = ahash_algs_register_all(device_data);
	if (ret) {
		dev_err(dev, "%s: ahash_algs_register_all() failed!\n",
			__func__);
		goto out_power;
	}

	dev_info(dev, "successfully registered\n");
	return 0;

out_power:
	hash_disable_power(device_data, false);

out_clk_unprepare:
	clk_unprepare(device_data->clk);

out_clk:
	clk_put(device_data->clk);

out_regulator:
	regulator_put(device_data->regulator);

out_unmap:
	iounmap(device_data->base);

out_free_mem:
	release_mem_region(res->start, resource_size(res));

out_kfree:
	kfree(device_data);
out:
	return ret;
}
