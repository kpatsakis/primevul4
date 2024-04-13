static void stop_data_traffic(struct acm *acm)
{
	int i;

	dev_dbg(&acm->control->dev, "%s\n", __func__);

	usb_kill_urb(acm->ctrlurb);
	for (i = 0; i < ACM_NW; i++)
		usb_kill_urb(acm->wb[i].urb);
	for (i = 0; i < acm->rx_buflimit; i++)
		usb_kill_urb(acm->read_urbs[i]);

	cancel_work_sync(&acm->work);
}
