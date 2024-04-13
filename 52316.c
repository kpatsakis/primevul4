int cdc_ncm_bind_common(struct usbnet *dev, struct usb_interface *intf, u8 data_altsetting, int drvflags)
{
	struct cdc_ncm_ctx *ctx;
	struct usb_driver *driver;
	u8 *buf;
	int len;
	int temp;
	u8 iface_no;
	struct usb_cdc_parsed_header hdr;

	ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	hrtimer_init(&ctx->tx_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	ctx->tx_timer.function = &cdc_ncm_tx_timer_cb;
	ctx->bh.data = (unsigned long)dev;
	ctx->bh.func = cdc_ncm_txpath_bh;
	atomic_set(&ctx->stop, 0);
	spin_lock_init(&ctx->mtx);

	/* store ctx pointer in device data field */
	dev->data[0] = (unsigned long)ctx;

	/* only the control interface can be successfully probed */
	ctx->control = intf;

	/* get some pointers */
	driver = driver_of(intf);
	buf = intf->cur_altsetting->extra;
	len = intf->cur_altsetting->extralen;

	/* parse through descriptors associated with control interface */
	cdc_parse_cdc_header(&hdr, intf, buf, len);

	if (hdr.usb_cdc_union_desc)
		ctx->data = usb_ifnum_to_if(dev->udev,
					    hdr.usb_cdc_union_desc->bSlaveInterface0);
	ctx->ether_desc = hdr.usb_cdc_ether_desc;
	ctx->func_desc = hdr.usb_cdc_ncm_desc;
	ctx->mbim_desc = hdr.usb_cdc_mbim_desc;
	ctx->mbim_extended_desc = hdr.usb_cdc_mbim_extended_desc;

	/* some buggy devices have an IAD but no CDC Union */
	if (!hdr.usb_cdc_union_desc && intf->intf_assoc && intf->intf_assoc->bInterfaceCount == 2) {
		ctx->data = usb_ifnum_to_if(dev->udev, intf->cur_altsetting->desc.bInterfaceNumber + 1);
		dev_dbg(&intf->dev, "CDC Union missing - got slave from IAD\n");
	}

	/* check if we got everything */
	if (!ctx->data) {
		dev_dbg(&intf->dev, "CDC Union missing and no IAD found\n");
		goto error;
	}
	if (cdc_ncm_comm_intf_is_mbim(intf->cur_altsetting)) {
		if (!ctx->mbim_desc) {
			dev_dbg(&intf->dev, "MBIM functional descriptor missing\n");
			goto error;
		}
	} else {
		if (!ctx->ether_desc || !ctx->func_desc) {
			dev_dbg(&intf->dev, "NCM or ECM functional descriptors missing\n");
			goto error;
		}
	}

	/* claim data interface, if different from control */
	if (ctx->data != ctx->control) {
		temp = usb_driver_claim_interface(driver, ctx->data, dev);
		if (temp) {
			dev_dbg(&intf->dev, "failed to claim data intf\n");
			goto error;
		}
	}

	iface_no = ctx->data->cur_altsetting->desc.bInterfaceNumber;

	/* Reset data interface. Some devices will not reset properly
	 * unless they are configured first.  Toggle the altsetting to
	 * force a reset
	 */
	usb_set_interface(dev->udev, iface_no, data_altsetting);
	temp = usb_set_interface(dev->udev, iface_no, 0);
	if (temp) {
		dev_dbg(&intf->dev, "set interface failed\n");
		goto error2;
	}

	/* initialize basic device settings */
	if (cdc_ncm_init(dev))
		goto error2;

	/* configure data interface */
	temp = usb_set_interface(dev->udev, iface_no, data_altsetting);
	if (temp) {
		dev_dbg(&intf->dev, "set interface failed\n");
		goto error2;
	}

	cdc_ncm_find_endpoints(dev, ctx->data);
	cdc_ncm_find_endpoints(dev, ctx->control);
	if (!dev->in || !dev->out || !dev->status) {
		dev_dbg(&intf->dev, "failed to collect endpoints\n");
		goto error2;
	}

	usb_set_intfdata(ctx->data, dev);
	usb_set_intfdata(ctx->control, dev);

	if (ctx->ether_desc) {
		temp = usbnet_get_ethernet_addr(dev, ctx->ether_desc->iMACAddress);
		if (temp) {
			dev_dbg(&intf->dev, "failed to get mac address\n");
			goto error2;
		}
		dev_info(&intf->dev, "MAC-Address: %pM\n", dev->net->dev_addr);
	}

	/* finish setting up the device specific data */
	cdc_ncm_setup(dev);

	/* Device-specific flags */
	ctx->drvflags = drvflags;

	/* Allocate the delayed NDP if needed. */
	if (ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END) {
		ctx->delayed_ndp16 = kzalloc(ctx->max_ndp_size, GFP_KERNEL);
		if (!ctx->delayed_ndp16)
			goto error2;
		dev_info(&intf->dev, "NDP will be placed at end of frame for this device.");
	}

	/* override ethtool_ops */
	dev->net->ethtool_ops = &cdc_ncm_ethtool_ops;

	/* add our sysfs attrs */
	dev->net->sysfs_groups[0] = &cdc_ncm_sysfs_attr_group;

	/* must handle MTU changes */
	dev->net->netdev_ops = &cdc_ncm_netdev_ops;

	return 0;

error2:
	usb_set_intfdata(ctx->control, NULL);
	usb_set_intfdata(ctx->data, NULL);
	if (ctx->data != ctx->control)
		usb_driver_release_interface(driver, ctx->data);
error:
	cdc_ncm_free((struct cdc_ncm_ctx *)dev->data[0]);
	dev->data[0] = 0;
	dev_info(&intf->dev, "bind() failure\n");
	return -ENODEV;
}
