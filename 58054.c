static void catc_disconnect(struct usb_interface *intf)
{
	struct catc *catc = usb_get_intfdata(intf);

	usb_set_intfdata(intf, NULL);
	if (catc) {
		unregister_netdev(catc->netdev);
		usb_free_urb(catc->ctrl_urb);
		usb_free_urb(catc->tx_urb);
		usb_free_urb(catc->rx_urb);
		usb_free_urb(catc->irq_urb);
		free_netdev(catc->netdev);
	}
}
