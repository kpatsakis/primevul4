int hid_check_keys_pressed(struct hid_device *hid)
{
	struct hid_input *hidinput;
	int i;

	if (!(hid->claimed & HID_CLAIMED_INPUT))
		return 0;

	list_for_each_entry(hidinput, &hid->inputs, list) {
		for (i = 0; i < BITS_TO_LONGS(KEY_MAX); i++)
			if (hidinput->input->key[i])
				return 1;
	}

	return 0;
}
