static int picolcd_probe_lcd(struct hid_device *hdev, struct picolcd_data *data)
{
	int error;

	/* Setup keypad input device */
	error = picolcd_init_keys(data, picolcd_in_report(REPORT_KEY_STATE, hdev));
	if (error)
		goto err;

	/* Setup CIR input device */
	error = picolcd_init_cir(data, picolcd_in_report(REPORT_IR_DATA, hdev));
	if (error)
		goto err;

	/* Set up the framebuffer device */
	error = picolcd_init_framebuffer(data);
	if (error)
		goto err;

	/* Setup lcd class device */
	error = picolcd_init_lcd(data, picolcd_out_report(REPORT_CONTRAST, hdev));
	if (error)
		goto err;

	/* Setup backlight class device */
	error = picolcd_init_backlight(data, picolcd_out_report(REPORT_BRIGHTNESS, hdev));
	if (error)
		goto err;

	/* Setup the LED class devices */
	error = picolcd_init_leds(data, picolcd_out_report(REPORT_LED_STATE, hdev));
	if (error)
		goto err;

	picolcd_init_devfs(data, picolcd_out_report(REPORT_EE_READ, hdev),
			picolcd_out_report(REPORT_EE_WRITE, hdev),
			picolcd_out_report(REPORT_READ_MEMORY, hdev),
			picolcd_out_report(REPORT_WRITE_MEMORY, hdev),
			picolcd_out_report(REPORT_RESET, hdev));
	return 0;
err:
	picolcd_exit_leds(data);
	picolcd_exit_backlight(data);
	picolcd_exit_lcd(data);
	picolcd_exit_framebuffer(data);
	picolcd_exit_cir(data);
	picolcd_exit_keys(data);
	return error;
}
