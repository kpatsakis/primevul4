static int airspy_free_urbs(struct airspy *s)
{
	int i;

	airspy_kill_urbs(s);

	for (i = s->urbs_initialized - 1; i >= 0; i--) {
		if (s->urb_list[i]) {
			dev_dbg(s->dev, "free urb=%d\n", i);
			/* free the URBs */
			usb_free_urb(s->urb_list[i]);
		}
	}
	s->urbs_initialized = 0;

	return 0;
}
