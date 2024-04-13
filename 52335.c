static void cdc_ncm_set_dgram_size(struct usbnet *dev, int new_size)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u8 iface_no = ctx->control->cur_altsetting->desc.bInterfaceNumber;
	__le16 max_datagram_size;
	u16 mbim_mtu;
	int err;

	/* set default based on descriptors */
	ctx->max_datagram_size = clamp_t(u32, new_size,
					 cdc_ncm_min_dgram_size(dev),
					 CDC_NCM_MAX_DATAGRAM_SIZE);

	/* inform the device about the selected Max Datagram Size? */
	if (!(cdc_ncm_flags(dev) & USB_CDC_NCM_NCAP_MAX_DATAGRAM_SIZE))
		goto out;

	/* read current mtu value from device */
	err = usbnet_read_cmd(dev, USB_CDC_GET_MAX_DATAGRAM_SIZE,
			      USB_TYPE_CLASS | USB_DIR_IN | USB_RECIP_INTERFACE,
			      0, iface_no, &max_datagram_size, 2);
	if (err < 0) {
		dev_dbg(&dev->intf->dev, "GET_MAX_DATAGRAM_SIZE failed\n");
		goto out;
	}

	if (le16_to_cpu(max_datagram_size) == ctx->max_datagram_size)
		goto out;

	max_datagram_size = cpu_to_le16(ctx->max_datagram_size);
	err = usbnet_write_cmd(dev, USB_CDC_SET_MAX_DATAGRAM_SIZE,
			       USB_TYPE_CLASS | USB_DIR_OUT | USB_RECIP_INTERFACE,
			       0, iface_no, &max_datagram_size, 2);
	if (err < 0)
		dev_dbg(&dev->intf->dev, "SET_MAX_DATAGRAM_SIZE failed\n");

out:
	/* set MTU to max supported by the device if necessary */
	dev->net->mtu = min_t(int, dev->net->mtu, ctx->max_datagram_size - cdc_ncm_eth_hlen(dev));

	/* do not exceed operater preferred MTU */
	if (ctx->mbim_extended_desc) {
		mbim_mtu = le16_to_cpu(ctx->mbim_extended_desc->wMTU);
		if (mbim_mtu != 0 && mbim_mtu < dev->net->mtu)
			dev->net->mtu = mbim_mtu;
	}
}
