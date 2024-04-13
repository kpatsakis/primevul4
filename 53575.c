static void hub_quiesce(struct usb_hub *hub, enum hub_quiescing_type type)
{
	struct usb_device *hdev = hub->hdev;
	int i;

	cancel_delayed_work_sync(&hub->init_work);

	/* hub_wq and related activity won't re-trigger */
	hub->quiescing = 1;

	if (type != HUB_SUSPEND) {
		/* Disconnect all the children */
		for (i = 0; i < hdev->maxchild; ++i) {
			if (hub->ports[i]->child)
				usb_disconnect(&hub->ports[i]->child);
		}
	}

	/* Stop hub_wq and related activity */
	usb_kill_urb(hub->urb);
	if (hub->has_indicators)
		cancel_delayed_work_sync(&hub->leds);
	if (hub->tt.hub)
		flush_work(&hub->tt.clear_work);
}
