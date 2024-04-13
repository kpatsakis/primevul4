static int picolcd_init_keys(struct picolcd_data *data,
		struct hid_report *report)
{
	struct hid_device *hdev = data->hdev;
	struct input_dev *idev;
	int error, i;

	if (!report)
		return -ENODEV;
	if (report->maxfield != 1 || report->field[0]->report_count != 2 ||
			report->field[0]->report_size != 8) {
		hid_err(hdev, "unsupported KEY_STATE report\n");
		return -EINVAL;
	}

	idev = input_allocate_device();
	if (idev == NULL) {
		hid_err(hdev, "failed to allocate input device\n");
		return -ENOMEM;
	}
	input_set_drvdata(idev, hdev);
	memcpy(data->keycode, def_keymap, sizeof(def_keymap));
	idev->name = hdev->name;
	idev->phys = hdev->phys;
	idev->uniq = hdev->uniq;
	idev->id.bustype = hdev->bus;
	idev->id.vendor  = hdev->vendor;
	idev->id.product = hdev->product;
	idev->id.version = hdev->version;
	idev->dev.parent = &hdev->dev;
	idev->keycode     = &data->keycode;
	idev->keycodemax  = PICOLCD_KEYS;
	idev->keycodesize = sizeof(data->keycode[0]);
	input_set_capability(idev, EV_MSC, MSC_SCAN);
	set_bit(EV_REP, idev->evbit);
	for (i = 0; i < PICOLCD_KEYS; i++)
		input_set_capability(idev, EV_KEY, data->keycode[i]);
	error = input_register_device(idev);
	if (error) {
		hid_err(hdev, "error registering the input device\n");
		input_free_device(idev);
		return error;
	}
	data->input_keys = idev;
	return 0;
}
