static int catc_open(struct net_device *netdev)
{
	struct catc *catc = netdev_priv(netdev);
	int status;

	catc->irq_urb->dev = catc->usbdev;
	if ((status = usb_submit_urb(catc->irq_urb, GFP_KERNEL)) < 0) {
		dev_err(&catc->usbdev->dev, "submit(irq_urb) status %d\n",
			status);
		return -1;
	}

	netif_start_queue(netdev);

	if (!catc->is_f5u011)
		mod_timer(&catc->timer, jiffies + STATS_UPDATE);

	return 0;
}
