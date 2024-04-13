static int picolcd_reset_resume(struct hid_device *hdev)
{
	int ret;
	ret = picolcd_reset(hdev);
	if (ret)
		dbg_hid(PICOLCD_NAME " resetting our device failed: %d\n", ret);
	ret = picolcd_fb_reset(hid_get_drvdata(hdev), 0);
	if (ret)
		dbg_hid(PICOLCD_NAME " restoring framebuffer content failed: %d\n", ret);
	ret = picolcd_resume_lcd(hid_get_drvdata(hdev));
	if (ret)
		dbg_hid(PICOLCD_NAME " restoring lcd failed: %d\n", ret);
	ret = picolcd_resume_backlight(hid_get_drvdata(hdev));
	if (ret)
		dbg_hid(PICOLCD_NAME " restoring backlight failed: %d\n", ret);
	picolcd_leds_set(hid_get_drvdata(hdev));
	return 0;
}
