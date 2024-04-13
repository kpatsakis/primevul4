static int catc_tx_run(struct catc *catc)
{
	int status;

	if (catc->is_f5u011)
		catc->tx_ptr = (catc->tx_ptr + 63) & ~63;

	catc->tx_urb->transfer_buffer_length = catc->tx_ptr;
	catc->tx_urb->transfer_buffer = catc->tx_buf[catc->tx_idx];
	catc->tx_urb->dev = catc->usbdev;

	if ((status = usb_submit_urb(catc->tx_urb, GFP_ATOMIC)) < 0)
		dev_err(&catc->usbdev->dev, "submit(tx_urb), status %d\n",
			status);

	catc->tx_idx = !catc->tx_idx;
	catc->tx_ptr = 0;

	netif_trans_update(catc->netdev);
	return status;
}
