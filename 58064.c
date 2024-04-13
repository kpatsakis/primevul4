static int catc_stop(struct net_device *netdev)
{
	struct catc *catc = netdev_priv(netdev);

	netif_stop_queue(netdev);

	if (!catc->is_f5u011)
		del_timer_sync(&catc->timer);

	usb_kill_urb(catc->rx_urb);
	usb_kill_urb(catc->tx_urb);
	usb_kill_urb(catc->irq_urb);
	usb_kill_urb(catc->ctrl_urb);

	return 0;
}
