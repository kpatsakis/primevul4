static int picolcd_raw_keypad(struct picolcd_data *data,
		struct hid_report *report, u8 *raw_data, int size)
{
	/*
	 * Keypad event
	 * First and second data bytes list currently pressed keys,
	 * 0x00 means no key and at most 2 keys may be pressed at same time
	 */
	int i, j;

	/* determine newly pressed keys */
	for (i = 0; i < size; i++) {
		unsigned int key_code;
		if (raw_data[i] == 0)
			continue;
		for (j = 0; j < sizeof(data->pressed_keys); j++)
			if (data->pressed_keys[j] == raw_data[i])
				goto key_already_down;
		for (j = 0; j < sizeof(data->pressed_keys); j++)
			if (data->pressed_keys[j] == 0) {
				data->pressed_keys[j] = raw_data[i];
				break;
			}
		input_event(data->input_keys, EV_MSC, MSC_SCAN, raw_data[i]);
		if (raw_data[i] < PICOLCD_KEYS)
			key_code = data->keycode[raw_data[i]];
		else
			key_code = KEY_UNKNOWN;
		if (key_code != KEY_UNKNOWN) {
			dbg_hid(PICOLCD_NAME " got key press for %u:%d",
					raw_data[i], key_code);
			input_report_key(data->input_keys, key_code, 1);
		}
		input_sync(data->input_keys);
key_already_down:
		continue;
	}

	/* determine newly released keys */
	for (j = 0; j < sizeof(data->pressed_keys); j++) {
		unsigned int key_code;
		if (data->pressed_keys[j] == 0)
			continue;
		for (i = 0; i < size; i++)
			if (data->pressed_keys[j] == raw_data[i])
				goto key_still_down;
		input_event(data->input_keys, EV_MSC, MSC_SCAN, data->pressed_keys[j]);
		if (data->pressed_keys[j] < PICOLCD_KEYS)
			key_code = data->keycode[data->pressed_keys[j]];
		else
			key_code = KEY_UNKNOWN;
		if (key_code != KEY_UNKNOWN) {
			dbg_hid(PICOLCD_NAME " got key release for %u:%d",
					data->pressed_keys[j], key_code);
			input_report_key(data->input_keys, key_code, 0);
		}
		input_sync(data->input_keys);
		data->pressed_keys[j] = 0;
key_still_down:
		continue;
	}
	return 1;
}
