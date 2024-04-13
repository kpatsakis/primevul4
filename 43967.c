static int lg_event(struct hid_device *hdev, struct hid_field *field,
		struct hid_usage *usage, __s32 value)
{
	struct lg_drv_data *drv_data = hid_get_drvdata(hdev);

	if ((drv_data->quirks & LG_INVERT_HWHEEL) && usage->code == REL_HWHEEL) {
		input_event(field->hidinput->input, usage->type, usage->code,
				-value);
		return 1;
	}
	if (drv_data->quirks & LG_FF4) {
		return lg4ff_adjust_input_event(hdev, field, usage, value, drv_data);
	}

	return 0;
}
