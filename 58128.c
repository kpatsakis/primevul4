static void gs_usb_receive_bulk_callback(struct urb *urb)
{
	struct gs_usb *usbcan = urb->context;
	struct gs_can *dev;
	struct net_device *netdev;
	int rc;
	struct net_device_stats *stats;
	struct gs_host_frame *hf = urb->transfer_buffer;
	struct gs_tx_context *txc;
	struct can_frame *cf;
	struct sk_buff *skb;

	BUG_ON(!usbcan);

	switch (urb->status) {
	case 0: /* success */
		break;
	case -ENOENT:
	case -ESHUTDOWN:
		return;
	default:
		/* do not resubmit aborted urbs. eg: when device goes down */
		return;
	}

	/* device reports out of range channel id */
	if (hf->channel >= GS_MAX_INTF)
		goto resubmit_urb;

	dev = usbcan->canch[hf->channel];

	netdev = dev->netdev;
	stats = &netdev->stats;

	if (!netif_device_present(netdev))
		return;

	if (hf->echo_id == -1) { /* normal rx */
		skb = alloc_can_skb(dev->netdev, &cf);
		if (!skb)
			return;

		cf->can_id = hf->can_id;

		cf->can_dlc = get_can_dlc(hf->can_dlc);
		memcpy(cf->data, hf->data, 8);

		/* ERROR frames tell us information about the controller */
		if (hf->can_id & CAN_ERR_FLAG)
			gs_update_state(dev, cf);

		netdev->stats.rx_packets++;
		netdev->stats.rx_bytes += hf->can_dlc;

		netif_rx(skb);
	} else { /* echo_id == hf->echo_id */
		if (hf->echo_id >= GS_MAX_TX_URBS) {
			netdev_err(netdev,
				   "Unexpected out of range echo id %d\n",
				   hf->echo_id);
			goto resubmit_urb;
		}

		netdev->stats.tx_packets++;
		netdev->stats.tx_bytes += hf->can_dlc;

		txc = gs_get_tx_context(dev, hf->echo_id);

		/* bad devices send bad echo_ids. */
		if (!txc) {
			netdev_err(netdev,
				   "Unexpected unused echo id %d\n",
				   hf->echo_id);
			goto resubmit_urb;
		}

		can_get_echo_skb(netdev, hf->echo_id);

		gs_free_tx_context(txc);

		netif_wake_queue(netdev);
	}

	if (hf->flags & GS_CAN_FLAG_OVERFLOW) {
		skb = alloc_can_err_skb(netdev, &cf);
		if (!skb)
			goto resubmit_urb;

		cf->can_id |= CAN_ERR_CRTL;
		cf->can_dlc = CAN_ERR_DLC;
		cf->data[1] = CAN_ERR_CRTL_RX_OVERFLOW;
		stats->rx_over_errors++;
		stats->rx_errors++;
		netif_rx(skb);
	}

 resubmit_urb:
	usb_fill_bulk_urb(urb,
			  usbcan->udev,
			  usb_rcvbulkpipe(usbcan->udev, GSUSB_ENDPOINT_IN),
			  hf,
			  sizeof(struct gs_host_frame),
			  gs_usb_receive_bulk_callback,
			  usbcan
			  );

	rc = usb_submit_urb(urb, GFP_ATOMIC);

	/* USB failure take down all interfaces */
	if (rc == -ENODEV) {
		for (rc = 0; rc < GS_MAX_INTF; rc++) {
			if (usbcan->canch[rc])
				netif_device_detach(usbcan->canch[rc]->netdev);
		}
	}
}
