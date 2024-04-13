static int cdc_ncm_init(struct usbnet *dev)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u8 iface_no = ctx->control->cur_altsetting->desc.bInterfaceNumber;
	int err;

	err = usbnet_read_cmd(dev, USB_CDC_GET_NTB_PARAMETERS,
			      USB_TYPE_CLASS | USB_DIR_IN
			      |USB_RECIP_INTERFACE,
			      0, iface_no, &ctx->ncm_parm,
			      sizeof(ctx->ncm_parm));
	if (err < 0) {
		dev_err(&dev->intf->dev, "failed GET_NTB_PARAMETERS\n");
		return err; /* GET_NTB_PARAMETERS is required */
	}

	/* set CRC Mode */
	if (cdc_ncm_flags(dev) & USB_CDC_NCM_NCAP_CRC_MODE) {
		dev_dbg(&dev->intf->dev, "Setting CRC mode off\n");
		err = usbnet_write_cmd(dev, USB_CDC_SET_CRC_MODE,
				       USB_TYPE_CLASS | USB_DIR_OUT
				       | USB_RECIP_INTERFACE,
				       USB_CDC_NCM_CRC_NOT_APPENDED,
				       iface_no, NULL, 0);
		if (err < 0)
			dev_err(&dev->intf->dev, "SET_CRC_MODE failed\n");
	}

	/* set NTB format, if both formats are supported.
	 *
	 * "The host shall only send this command while the NCM Data
	 *  Interface is in alternate setting 0."
	 */
	if (le16_to_cpu(ctx->ncm_parm.bmNtbFormatsSupported) &
						USB_CDC_NCM_NTB32_SUPPORTED) {
		dev_dbg(&dev->intf->dev, "Setting NTB format to 16-bit\n");
		err = usbnet_write_cmd(dev, USB_CDC_SET_NTB_FORMAT,
				       USB_TYPE_CLASS | USB_DIR_OUT
				       | USB_RECIP_INTERFACE,
				       USB_CDC_NCM_NTB16_FORMAT,
				       iface_no, NULL, 0);
		if (err < 0)
			dev_err(&dev->intf->dev, "SET_NTB_FORMAT failed\n");
	}

	/* set initial device values */
	ctx->rx_max = le32_to_cpu(ctx->ncm_parm.dwNtbInMaxSize);
	ctx->tx_max = le32_to_cpu(ctx->ncm_parm.dwNtbOutMaxSize);
	ctx->tx_remainder = le16_to_cpu(ctx->ncm_parm.wNdpOutPayloadRemainder);
	ctx->tx_modulus = le16_to_cpu(ctx->ncm_parm.wNdpOutDivisor);
	ctx->tx_ndp_modulus = le16_to_cpu(ctx->ncm_parm.wNdpOutAlignment);
	/* devices prior to NCM Errata shall set this field to zero */
	ctx->tx_max_datagrams = le16_to_cpu(ctx->ncm_parm.wNtbOutMaxDatagrams);

	dev_dbg(&dev->intf->dev,
		"dwNtbInMaxSize=%u dwNtbOutMaxSize=%u wNdpOutPayloadRemainder=%u wNdpOutDivisor=%u wNdpOutAlignment=%u wNtbOutMaxDatagrams=%u flags=0x%x\n",
		ctx->rx_max, ctx->tx_max, ctx->tx_remainder, ctx->tx_modulus,
		ctx->tx_ndp_modulus, ctx->tx_max_datagrams, cdc_ncm_flags(dev));

	/* max count of tx datagrams */
	if ((ctx->tx_max_datagrams == 0) ||
			(ctx->tx_max_datagrams > CDC_NCM_DPT_DATAGRAMS_MAX))
		ctx->tx_max_datagrams = CDC_NCM_DPT_DATAGRAMS_MAX;

	/* set up maximum NDP size */
	ctx->max_ndp_size = sizeof(struct usb_cdc_ncm_ndp16) + (ctx->tx_max_datagrams + 1) * sizeof(struct usb_cdc_ncm_dpe16);

	/* initial coalescing timer interval */
	ctx->timer_interval = CDC_NCM_TIMER_INTERVAL_USEC * NSEC_PER_USEC;

	return 0;
}
