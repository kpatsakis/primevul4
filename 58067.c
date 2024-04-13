static void catc_tx_timeout(struct net_device *netdev)
{
	struct catc *catc = netdev_priv(netdev);

	dev_warn(&netdev->dev, "Transmit timed out.\n");
	usb_unlink_urb(catc->tx_urb);
}
