static int airspy_kill_urbs(struct airspy *s)
{
	int i;

	for (i = s->urbs_submitted - 1; i >= 0; i--) {
		dev_dbg(s->dev, "kill urb=%d\n", i);
		/* stop the URB */
		usb_kill_urb(s->urb_list[i]);
	}
	s->urbs_submitted = 0;

	return 0;
}
