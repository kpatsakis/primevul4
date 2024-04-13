static void cdc_ncm_update_rxtx_max(struct usbnet *dev, u32 new_rx, u32 new_tx)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u8 iface_no = ctx->control->cur_altsetting->desc.bInterfaceNumber;
	u32 val;

	val = cdc_ncm_check_rx_max(dev, new_rx);

	/* inform device about NTB input size changes */
	if (val != ctx->rx_max) {
		__le32 dwNtbInMaxSize = cpu_to_le32(val);

		dev_info(&dev->intf->dev, "setting rx_max = %u\n", val);

		/* tell device to use new size */
		if (usbnet_write_cmd(dev, USB_CDC_SET_NTB_INPUT_SIZE,
				     USB_TYPE_CLASS | USB_DIR_OUT
				     | USB_RECIP_INTERFACE,
				     0, iface_no, &dwNtbInMaxSize, 4) < 0)
			dev_dbg(&dev->intf->dev, "Setting NTB Input Size failed\n");
		else
			ctx->rx_max = val;
	}

	/* usbnet use these values for sizing rx queues */
	if (dev->rx_urb_size != ctx->rx_max) {
		dev->rx_urb_size = ctx->rx_max;
		if (netif_running(dev->net))
			usbnet_unlink_rx_urbs(dev);
	}

	val = cdc_ncm_check_tx_max(dev, new_tx);
	if (val != ctx->tx_max)
		dev_info(&dev->intf->dev, "setting tx_max = %u\n", val);

	/* Adding a pad byte here if necessary simplifies the handling
	 * in cdc_ncm_fill_tx_frame, making tx_max always represent
	 * the real skb max size.
	 *
	 * We cannot use dev->maxpacket here because this is called from
	 * .bind which is called before usbnet sets up dev->maxpacket
	 */
	if (val != le32_to_cpu(ctx->ncm_parm.dwNtbOutMaxSize) &&
	    val % usb_maxpacket(dev->udev, dev->out, 1) == 0)
		val++;

	/* we might need to flush any pending tx buffers if running */
	if (netif_running(dev->net) && val > ctx->tx_max) {
		netif_tx_lock_bh(dev->net);
		usbnet_start_xmit(NULL, dev->net);
		/* make sure tx_curr_skb is reallocated if it was empty */
		if (ctx->tx_curr_skb) {
			dev_kfree_skb_any(ctx->tx_curr_skb);
			ctx->tx_curr_skb = NULL;
		}
		ctx->tx_max = val;
		netif_tx_unlock_bh(dev->net);
	} else {
		ctx->tx_max = val;
	}

	dev->hard_mtu = ctx->tx_max;

	/* max qlen depend on hard_mtu and rx_urb_size */
	usbnet_update_max_qlen(dev);

	/* never pad more than 3 full USB packets per transfer */
	ctx->min_tx_pkt = clamp_t(u16, ctx->tx_max - 3 * usb_maxpacket(dev->udev, dev->out, 1),
				  CDC_NCM_MIN_TX_PKT, ctx->tx_max);
}
