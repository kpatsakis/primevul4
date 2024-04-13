static int pmu_dev_alloc(struct pmu *pmu)
{
	int ret = -ENOMEM;

	pmu->dev = kzalloc(sizeof(struct device), GFP_KERNEL);
	if (!pmu->dev)
		goto out;

	pmu->dev->groups = pmu->attr_groups;
	device_initialize(pmu->dev);
	ret = dev_set_name(pmu->dev, "%s", pmu->name);
	if (ret)
		goto free_dev;

	dev_set_drvdata(pmu->dev, pmu);
	pmu->dev->bus = &pmu_bus;
	pmu->dev->release = pmu_dev_release;
	ret = device_add(pmu->dev);
	if (ret)
		goto free_dev;

	/* For PMUs with address filters, throw in an extra attribute: */
	if (pmu->nr_addr_filters)
		ret = device_create_file(pmu->dev, &dev_attr_nr_addr_filters);

	if (ret)
		goto del_dev;

out:
	return ret;

del_dev:
	device_del(pmu->dev);

free_dev:
	put_device(pmu->dev);
	goto out;
}
