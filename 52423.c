static int ims_pcu_setup_backlight(struct ims_pcu *pcu)
{
	struct ims_pcu_backlight *backlight = &pcu->backlight;
	int error;

	INIT_WORK(&backlight->work, ims_pcu_backlight_work);
	snprintf(backlight->name, sizeof(backlight->name),
		 "pcu%d::kbd_backlight", pcu->device_no);

	backlight->cdev.name = backlight->name;
	backlight->cdev.max_brightness = IMS_PCU_MAX_BRIGHTNESS;
	backlight->cdev.brightness_get = ims_pcu_backlight_get_brightness;
	backlight->cdev.brightness_set = ims_pcu_backlight_set_brightness;

	error = led_classdev_register(pcu->dev, &backlight->cdev);
	if (error) {
		dev_err(pcu->dev,
			"Failed to register backlight LED device, error: %d\n",
			error);
		return error;
	}

	return 0;
}
