static int picolcd_suspend(struct hid_device *hdev, pm_message_t message)
{
	if (PMSG_IS_AUTO(message))
		return 0;

	picolcd_suspend_backlight(hid_get_drvdata(hdev));
	dbg_hid(PICOLCD_NAME " device ready for suspend\n");
	return 0;
}
