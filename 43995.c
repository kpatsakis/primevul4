static int magicmouse_probe(struct hid_device *hdev,
	const struct hid_device_id *id)
{
	__u8 feature[] = { 0xd7, 0x01 };
	struct magicmouse_sc *msc;
	struct hid_report *report;
	int ret;

	msc = devm_kzalloc(&hdev->dev, sizeof(*msc), GFP_KERNEL);
	if (msc == NULL) {
		hid_err(hdev, "can't alloc magicmouse descriptor\n");
		return -ENOMEM;
	}

	msc->scroll_accel = SCROLL_ACCEL_DEFAULT;

	msc->quirks = id->driver_data;
	hid_set_drvdata(hdev, msc);

	ret = hid_parse(hdev);
	if (ret) {
		hid_err(hdev, "magicmouse hid parse failed\n");
		return ret;
	}

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (ret) {
		hid_err(hdev, "magicmouse hw start failed\n");
		return ret;
	}

	if (!msc->input) {
		hid_err(hdev, "magicmouse input not registered\n");
		ret = -ENOMEM;
		goto err_stop_hw;
	}

	if (id->product == USB_DEVICE_ID_APPLE_MAGICMOUSE)
		report = hid_register_report(hdev, HID_INPUT_REPORT,
			MOUSE_REPORT_ID);
	else { /* USB_DEVICE_ID_APPLE_MAGICTRACKPAD */
		report = hid_register_report(hdev, HID_INPUT_REPORT,
			TRACKPAD_REPORT_ID);
		report = hid_register_report(hdev, HID_INPUT_REPORT,
			DOUBLE_REPORT_ID);
	}

	if (!report) {
		hid_err(hdev, "unable to register touch report\n");
		ret = -ENOMEM;
		goto err_stop_hw;
	}
	report->size = 6;

	/*
	 * Some devices repond with 'invalid report id' when feature
	 * report switching it into multitouch mode is sent to it.
	 *
	 * This results in -EIO from the _raw low-level transport callback,
	 * but there seems to be no other way of switching the mode.
	 * Thus the super-ugly hacky success check below.
	 */
	ret = hid_hw_raw_request(hdev, feature[0], feature, sizeof(feature),
				HID_FEATURE_REPORT, HID_REQ_SET_REPORT);
	if (ret != -EIO && ret != sizeof(feature)) {
		hid_err(hdev, "unable to request touch data (%d)\n", ret);
		goto err_stop_hw;
	}

	return 0;
err_stop_hw:
	hid_hw_stop(hdev);
	return ret;
}
