static int lg_input_mapped(struct hid_device *hdev, struct hid_input *hi,
		struct hid_field *field, struct hid_usage *usage,
		unsigned long **bit, int *max)
{
	struct lg_drv_data *drv_data = hid_get_drvdata(hdev);

	if ((drv_data->quirks & LG_BAD_RELATIVE_KEYS) && usage->type == EV_KEY &&
			(field->flags & HID_MAIN_ITEM_RELATIVE))
		field->flags &= ~HID_MAIN_ITEM_RELATIVE;

	if ((drv_data->quirks & LG_DUPLICATE_USAGES) && (usage->type == EV_KEY ||
			 usage->type == EV_REL || usage->type == EV_ABS))
		clear_bit(usage->code, *bit);

	/* Ensure that Logitech wheels are not given a default fuzz/flat value */
	if (usage->type == EV_ABS && (usage->code == ABS_X ||
			usage->code == ABS_Y || usage->code == ABS_Z ||
			usage->code == ABS_RZ)) {
		switch (hdev->product) {
		case USB_DEVICE_ID_LOGITECH_WHEEL:
		case USB_DEVICE_ID_LOGITECH_MOMO_WHEEL:
		case USB_DEVICE_ID_LOGITECH_DFP_WHEEL:
		case USB_DEVICE_ID_LOGITECH_G25_WHEEL:
		case USB_DEVICE_ID_LOGITECH_DFGT_WHEEL:
		case USB_DEVICE_ID_LOGITECH_G27_WHEEL:
		case USB_DEVICE_ID_LOGITECH_WII_WHEEL:
		case USB_DEVICE_ID_LOGITECH_MOMO_WHEEL2:
		case USB_DEVICE_ID_LOGITECH_VIBRATION_WHEEL:
			field->application = HID_GD_MULTIAXIS;
			break;
		default:
			break;
		}
	}

	return 0;
}
