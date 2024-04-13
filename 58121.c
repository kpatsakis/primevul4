static netdev_tx_t gs_can_start_xmit(struct sk_buff *skb,
				     struct net_device *netdev)
{
	struct gs_can *dev = netdev_priv(netdev);
	struct net_device_stats *stats = &dev->netdev->stats;
	struct urb *urb;
	struct gs_host_frame *hf;
	struct can_frame *cf;
	int rc;
	unsigned int idx;
	struct gs_tx_context *txc;

	if (can_dropped_invalid_skb(netdev, skb))
		return NETDEV_TX_OK;

	/* find an empty context to keep track of transmission */
	txc = gs_alloc_tx_context(dev);
	if (!txc)
		return NETDEV_TX_BUSY;

	/* create a URB, and a buffer for it */
	urb = usb_alloc_urb(0, GFP_ATOMIC);
	if (!urb)
		goto nomem_urb;

	hf = usb_alloc_coherent(dev->udev, sizeof(*hf), GFP_ATOMIC,
				&urb->transfer_dma);
	if (!hf) {
		netdev_err(netdev, "No memory left for USB buffer\n");
		goto nomem_hf;
	}

	idx = txc->echo_id;

	if (idx >= GS_MAX_TX_URBS) {
		netdev_err(netdev, "Invalid tx context %d\n", idx);
		goto badidx;
	}

	hf->echo_id = idx;
	hf->channel = dev->channel;

	cf = (struct can_frame *)skb->data;

	hf->can_id = cf->can_id;
	hf->can_dlc = cf->can_dlc;
	memcpy(hf->data, cf->data, cf->can_dlc);

	usb_fill_bulk_urb(urb, dev->udev,
			  usb_sndbulkpipe(dev->udev, GSUSB_ENDPOINT_OUT),
			  hf,
			  sizeof(*hf),
			  gs_usb_xmit_callback,
			  txc);

	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	usb_anchor_urb(urb, &dev->tx_submitted);

	can_put_echo_skb(skb, netdev, idx);

	atomic_inc(&dev->active_tx_urbs);

	rc = usb_submit_urb(urb, GFP_ATOMIC);
	if (unlikely(rc)) {			/* usb send failed */
		atomic_dec(&dev->active_tx_urbs);

		can_free_echo_skb(netdev, idx);
		gs_free_tx_context(txc);

		usb_unanchor_urb(urb);
		usb_free_coherent(dev->udev,
				  sizeof(*hf),
				  hf,
				  urb->transfer_dma);


		if (rc == -ENODEV) {
			netif_device_detach(netdev);
		} else {
			netdev_err(netdev, "usb_submit failed (err=%d)\n", rc);
			stats->tx_dropped++;
		}
	} else {
		/* Slow down tx path */
		if (atomic_read(&dev->active_tx_urbs) >= GS_MAX_TX_URBS)
			netif_stop_queue(netdev);
	}

	/* let usb core take care of this urb */
	usb_free_urb(urb);

	return NETDEV_TX_OK;

 badidx:
	usb_free_coherent(dev->udev,
			  sizeof(*hf),
			  hf,
			  urb->transfer_dma);
 nomem_hf:
	usb_free_urb(urb);

 nomem_urb:
	gs_free_tx_context(txc);
	dev_kfree_skb(skb);
	stats->tx_dropped++;
	return NETDEV_TX_OK;
}
