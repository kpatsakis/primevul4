static int uas_wait_for_pending_cmnds(struct uas_dev_info *devinfo)
{
	unsigned long start_time;
	int r;

	start_time = jiffies;
	do {
		flush_work(&devinfo->work);

		r = usb_wait_anchor_empty_timeout(&devinfo->sense_urbs, 5000);
		if (r == 0)
			return -ETIME;

		r = usb_wait_anchor_empty_timeout(&devinfo->data_urbs, 500);
		if (r == 0)
			return -ETIME;

		if (time_after(jiffies, start_time + 5 * HZ))
			return -ETIME;
	} while (!uas_cmnd_list_empty(devinfo));

	return 0;
}
