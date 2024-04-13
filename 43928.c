static int picolcd_probe(struct hid_device *hdev,
		     const struct hid_device_id *id)
{
	struct picolcd_data *data;
	int error = -ENOMEM;

	dbg_hid(PICOLCD_NAME " hardware probe...\n");

	/*
	 * Let's allocate the picolcd data structure, set some reasonable
	 * defaults, and associate it with the device
	 */
	data = kzalloc(sizeof(struct picolcd_data), GFP_KERNEL);
	if (data == NULL) {
		hid_err(hdev, "can't allocate space for Minibox PicoLCD device data\n");
		error = -ENOMEM;
		goto err_no_cleanup;
	}

	spin_lock_init(&data->lock);
	mutex_init(&data->mutex);
	data->hdev = hdev;
	data->opmode_delay = 5000;
	if (hdev->product == USB_DEVICE_ID_PICOLCD_BOOTLOADER)
		data->status |= PICOLCD_BOOTLOADER;
	hid_set_drvdata(hdev, data);

	/* Parse the device reports and start it up */
	error = hid_parse(hdev);
	if (error) {
		hid_err(hdev, "device report parse failed\n");
		goto err_cleanup_data;
	}

	error = hid_hw_start(hdev, 0);
	if (error) {
		hid_err(hdev, "hardware start failed\n");
		goto err_cleanup_data;
	}

	error = hid_hw_open(hdev);
	if (error) {
		hid_err(hdev, "failed to open input interrupt pipe for key and IR events\n");
		goto err_cleanup_hid_hw;
	}

	error = device_create_file(&hdev->dev, &dev_attr_operation_mode_delay);
	if (error) {
		hid_err(hdev, "failed to create sysfs attributes\n");
		goto err_cleanup_hid_ll;
	}

	error = device_create_file(&hdev->dev, &dev_attr_operation_mode);
	if (error) {
		hid_err(hdev, "failed to create sysfs attributes\n");
		goto err_cleanup_sysfs1;
	}

	if (data->status & PICOLCD_BOOTLOADER)
		error = picolcd_probe_bootloader(hdev, data);
	else
		error = picolcd_probe_lcd(hdev, data);
	if (error)
		goto err_cleanup_sysfs2;

	dbg_hid(PICOLCD_NAME " activated and initialized\n");
	return 0;

err_cleanup_sysfs2:
	device_remove_file(&hdev->dev, &dev_attr_operation_mode);
err_cleanup_sysfs1:
	device_remove_file(&hdev->dev, &dev_attr_operation_mode_delay);
err_cleanup_hid_ll:
	hid_hw_close(hdev);
err_cleanup_hid_hw:
	hid_hw_stop(hdev);
err_cleanup_data:
	kfree(data);
err_no_cleanup:
	hid_set_drvdata(hdev, NULL);

	return error;
}
