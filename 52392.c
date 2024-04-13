static void ims_pcu_buttons_report(struct ims_pcu *pcu, u32 data)
{
	struct ims_pcu_buttons *buttons = &pcu->buttons;
	struct input_dev *input = buttons->input;
	int i;

	for (i = 0; i < 32; i++) {
		unsigned short keycode = buttons->keymap[i];

		if (keycode != KEY_RESERVED)
			input_report_key(input, keycode, data & (1UL << i));
	}

	input_sync(input);
}
