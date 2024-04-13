static int ims_pcu_setup_gamepad(struct ims_pcu *pcu)
{
	struct ims_pcu_gamepad *gamepad;
	struct input_dev *input;
	int error;

	gamepad = kzalloc(sizeof(struct ims_pcu_gamepad), GFP_KERNEL);
	input = input_allocate_device();
	if (!gamepad || !input) {
		dev_err(pcu->dev,
			"Not enough memory for gamepad device\n");
		error = -ENOMEM;
		goto err_free_mem;
	}

	gamepad->input = input;

	snprintf(gamepad->name, sizeof(gamepad->name),
		 "IMS PCU#%d Gamepad Interface", pcu->device_no);

	usb_make_path(pcu->udev, gamepad->phys, sizeof(gamepad->phys));
	strlcat(gamepad->phys, "/input1", sizeof(gamepad->phys));

	input->name = gamepad->name;
	input->phys = gamepad->phys;
	usb_to_input_id(pcu->udev, &input->id);
	input->dev.parent = &pcu->ctrl_intf->dev;

	__set_bit(EV_KEY, input->evbit);
	__set_bit(BTN_A, input->keybit);
	__set_bit(BTN_B, input->keybit);
	__set_bit(BTN_X, input->keybit);
	__set_bit(BTN_Y, input->keybit);
	__set_bit(BTN_START, input->keybit);
	__set_bit(BTN_SELECT, input->keybit);

	__set_bit(EV_ABS, input->evbit);
	input_set_abs_params(input, ABS_X, -1, 1, 0, 0);
	input_set_abs_params(input, ABS_Y, -1, 1, 0, 0);

	error = input_register_device(input);
	if (error) {
		dev_err(pcu->dev,
			"Failed to register gamepad input device: %d\n",
			error);
		goto err_free_mem;
	}

	pcu->gamepad = gamepad;
	return 0;

err_free_mem:
	input_free_device(input);
	kfree(gamepad);
	return -ENOMEM;
}
