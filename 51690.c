static int airspy_submit_urbs(struct airspy *s)
{
	int i, ret;

	for (i = 0; i < s->urbs_initialized; i++) {
		dev_dbg(s->dev, "submit urb=%d\n", i);
		ret = usb_submit_urb(s->urb_list[i], GFP_ATOMIC);
		if (ret) {
			dev_err(s->dev, "Could not submit URB no. %d - get them all back\n",
					i);
			airspy_kill_urbs(s);
			return ret;
		}
		s->urbs_submitted++;
	}

	return 0;
}
