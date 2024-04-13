static void ati_remote2_input_mouse(struct ati_remote2 *ar2)
{
	struct input_dev *idev = ar2->idev;
	u8 *data = ar2->buf[0];
	int channel, mode;

	channel = data[0] >> 4;

	if (!((1 << channel) & ar2->channel_mask))
		return;

	mode = data[0] & 0x0F;

	if (mode > ATI_REMOTE2_PC) {
		dev_err(&ar2->intf[0]->dev,
			"Unknown mode byte (%02x %02x %02x %02x)\n",
			data[3], data[2], data[1], data[0]);
		return;
	}

	if (!((1 << mode) & ar2->mode_mask))
		return;

	input_event(idev, EV_REL, REL_X, (s8) data[1]);
	input_event(idev, EV_REL, REL_Y, (s8) data[2]);
	input_sync(idev);
}
