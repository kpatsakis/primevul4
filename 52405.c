static int ims_pcu_init_application_mode(struct ims_pcu *pcu)
{
	static atomic_t device_no = ATOMIC_INIT(-1);

	const struct ims_pcu_device_info *info;
	int error;

	error = ims_pcu_get_device_info(pcu);
	if (error) {
		/* Device does not respond to basic queries, hopeless */
		return error;
	}

	error = ims_pcu_identify_type(pcu, &pcu->device_id);
	if (error) {
		dev_err(pcu->dev,
			"Failed to identify device, error: %d\n", error);
		/*
		 * Do not signal error, but do not create input nor
		 * backlight devices either, let userspace figure this
		 * out (flash a new firmware?).
		 */
		return 0;
	}

	if (pcu->device_id >= ARRAY_SIZE(ims_pcu_device_info) ||
	    !ims_pcu_device_info[pcu->device_id].keymap) {
		dev_err(pcu->dev, "Device ID %d is not valid\n", pcu->device_id);
		/* Same as above, punt to userspace */
		return 0;
	}

	/* Device appears to be operable, complete initialization */
	pcu->device_no = atomic_inc_return(&device_no);

	/*
	 * PCU-B devices, both GEN_1 and GEN_2 do not have OFN sensor
	 */
	if (pcu->device_id != IMS_PCU_PCU_B_DEVICE_ID) {
		error = sysfs_create_group(&pcu->dev->kobj,
					   &ims_pcu_ofn_attr_group);
		if (error)
			return error;
	}

	error = ims_pcu_setup_backlight(pcu);
	if (error)
		return error;

	info = &ims_pcu_device_info[pcu->device_id];
	error = ims_pcu_setup_buttons(pcu, info->keymap, info->keymap_len);
	if (error)
		goto err_destroy_backlight;

	if (info->has_gamepad) {
		error = ims_pcu_setup_gamepad(pcu);
		if (error)
			goto err_destroy_buttons;
	}

	pcu->setup_complete = true;

	return 0;

err_destroy_buttons:
	ims_pcu_destroy_buttons(pcu);
err_destroy_backlight:
	ims_pcu_destroy_backlight(pcu);
	return error;
}
