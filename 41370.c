static void tg3_hwmon_open(struct tg3 *tp)
{
	int i, err;
	u32 size = 0;
	struct pci_dev *pdev = tp->pdev;
	struct tg3_ocir ocirs[TG3_SD_NUM_RECS];

	tg3_sd_scan_scratchpad(tp, ocirs);

	for (i = 0; i < TG3_SD_NUM_RECS; i++) {
		if (!ocirs[i].src_data_length)
			continue;

		size += ocirs[i].src_hdr_length;
		size += ocirs[i].src_data_length;
	}

	if (!size)
		return;

	/* Register hwmon sysfs hooks */
	err = sysfs_create_group(&pdev->dev.kobj, &tg3_group);
	if (err) {
		dev_err(&pdev->dev, "Cannot create sysfs group, aborting\n");
		return;
	}

	tp->hwmon_dev = hwmon_device_register(&pdev->dev);
	if (IS_ERR(tp->hwmon_dev)) {
		tp->hwmon_dev = NULL;
		dev_err(&pdev->dev, "Cannot register hwmon device, aborting\n");
		sysfs_remove_group(&pdev->dev.kobj, &tg3_group);
	}
}
