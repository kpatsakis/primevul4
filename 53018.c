static int ati_remote2_getkeycode(struct input_dev *idev,
				  struct input_keymap_entry *ke)
{
	struct ati_remote2 *ar2 = input_get_drvdata(idev);
	unsigned int mode;
	int offset;
	unsigned int index;
	unsigned int scancode;

	if (ke->flags & INPUT_KEYMAP_BY_INDEX) {
		index = ke->index;
		if (index >= ATI_REMOTE2_MODES *
				ARRAY_SIZE(ati_remote2_key_table))
			return -EINVAL;

		mode = ke->index / ARRAY_SIZE(ati_remote2_key_table);
		offset = ke->index % ARRAY_SIZE(ati_remote2_key_table);
		scancode = (mode << 8) + ati_remote2_key_table[offset].hw_code;
	} else {
		if (input_scancode_to_scalar(ke, &scancode))
			return -EINVAL;

		mode = scancode >> 8;
		if (mode > ATI_REMOTE2_PC)
			return -EINVAL;

		offset = ati_remote2_lookup(scancode & 0xff);
		if (offset < 0)
			return -EINVAL;

		index = mode * ARRAY_SIZE(ati_remote2_key_table) + offset;
	}

	ke->keycode = ar2->keycode[mode][offset];
	ke->len = sizeof(scancode);
	memcpy(&ke->scancode, &scancode, sizeof(scancode));
	ke->index = index;

	return 0;
}
