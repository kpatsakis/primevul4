static void hidp_input_report(struct hidp_session *session, struct sk_buff *skb)
{
	struct input_dev *dev = session->input;
	unsigned char *keys = session->keys;
	unsigned char *udata = skb->data + 1;
	signed char *sdata = skb->data + 1;
	int i, size = skb->len - 1;

	switch (skb->data[0]) {
	case 0x01:	/* Keyboard report */
		for (i = 0; i < 8; i++)
			input_report_key(dev, hidp_keycode[i + 224], (udata[0] >> i) & 1);

		/* If all the key codes have been set to 0x01, it means
		 * too many keys were pressed at the same time. */
		if (!memcmp(udata + 2, hidp_mkeyspat, 6))
			break;

		for (i = 2; i < 8; i++) {
			if (keys[i] > 3 && memscan(udata + 2, keys[i], 6) == udata + 8) {
				if (hidp_keycode[keys[i]])
					input_report_key(dev, hidp_keycode[keys[i]], 0);
				else
					BT_ERR("Unknown key (scancode %#x) released.", keys[i]);
			}

			if (udata[i] > 3 && memscan(keys + 2, udata[i], 6) == keys + 8) {
				if (hidp_keycode[udata[i]])
					input_report_key(dev, hidp_keycode[udata[i]], 1);
				else
					BT_ERR("Unknown key (scancode %#x) pressed.", udata[i]);
			}
		}

		memcpy(keys, udata, 8);
		break;

	case 0x02:	/* Mouse report */
		input_report_key(dev, BTN_LEFT,   sdata[0] & 0x01);
		input_report_key(dev, BTN_RIGHT,  sdata[0] & 0x02);
		input_report_key(dev, BTN_MIDDLE, sdata[0] & 0x04);
		input_report_key(dev, BTN_SIDE,   sdata[0] & 0x08);
		input_report_key(dev, BTN_EXTRA,  sdata[0] & 0x10);

		input_report_rel(dev, REL_X, sdata[1]);
		input_report_rel(dev, REL_Y, sdata[2]);

		if (size > 3)
			input_report_rel(dev, REL_WHEEL, sdata[3]);
		break;
	}

	input_sync(dev);
}
