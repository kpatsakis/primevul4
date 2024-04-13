int picolcd_reset(struct hid_device *hdev)
{
	struct picolcd_data *data = hid_get_drvdata(hdev);
	struct hid_report *report = picolcd_out_report(REPORT_RESET, hdev);
	unsigned long flags;
	int error;

	if (!data || !report || report->maxfield != 1)
		return -ENODEV;

	spin_lock_irqsave(&data->lock, flags);
	if (hdev->product == USB_DEVICE_ID_PICOLCD_BOOTLOADER)
		data->status |= PICOLCD_BOOTLOADER;

	/* perform the reset */
	hid_set_field(report->field[0], 0, 1);
	if (data->status & PICOLCD_FAILED) {
		spin_unlock_irqrestore(&data->lock, flags);
		return -ENODEV;
	}
	hid_hw_request(hdev, report, HID_REQ_SET_REPORT);
	spin_unlock_irqrestore(&data->lock, flags);

	error = picolcd_check_version(hdev);
	if (error)
		return error;

	picolcd_resume_lcd(data);
	picolcd_resume_backlight(data);
	picolcd_fb_refresh(data);
	picolcd_leds_set(data);
	return 0;
}
