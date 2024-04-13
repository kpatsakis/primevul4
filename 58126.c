static struct gs_can *gs_make_candev(unsigned int channel,
				     struct usb_interface *intf,
				     struct gs_device_config *dconf)
{
	struct gs_can *dev;
	struct net_device *netdev;
	int rc;
	struct gs_device_bt_const *bt_const;

	bt_const = kmalloc(sizeof(*bt_const), GFP_KERNEL);
	if (!bt_const)
		return ERR_PTR(-ENOMEM);

	/* fetch bit timing constants */
	rc = usb_control_msg(interface_to_usbdev(intf),
			     usb_rcvctrlpipe(interface_to_usbdev(intf), 0),
			     GS_USB_BREQ_BT_CONST,
			     USB_DIR_IN|USB_TYPE_VENDOR|USB_RECIP_INTERFACE,
			     channel,
			     0,
			     bt_const,
			     sizeof(*bt_const),
			     1000);

	if (rc < 0) {
		dev_err(&intf->dev,
			"Couldn't get bit timing const for channel (err=%d)\n",
			rc);
		kfree(bt_const);
		return ERR_PTR(rc);
	}

	/* create netdev */
	netdev = alloc_candev(sizeof(struct gs_can), GS_MAX_TX_URBS);
	if (!netdev) {
		dev_err(&intf->dev, "Couldn't allocate candev\n");
		kfree(bt_const);
		return ERR_PTR(-ENOMEM);
	}

	dev = netdev_priv(netdev);

	netdev->netdev_ops = &gs_usb_netdev_ops;

	netdev->flags |= IFF_ECHO; /* we support full roundtrip echo */

	/* dev settup */
	strcpy(dev->bt_const.name, "gs_usb");
	dev->bt_const.tseg1_min = bt_const->tseg1_min;
	dev->bt_const.tseg1_max = bt_const->tseg1_max;
	dev->bt_const.tseg2_min = bt_const->tseg2_min;
	dev->bt_const.tseg2_max = bt_const->tseg2_max;
	dev->bt_const.sjw_max = bt_const->sjw_max;
	dev->bt_const.brp_min = bt_const->brp_min;
	dev->bt_const.brp_max = bt_const->brp_max;
	dev->bt_const.brp_inc = bt_const->brp_inc;

	dev->udev = interface_to_usbdev(intf);
	dev->iface = intf;
	dev->netdev = netdev;
	dev->channel = channel;

	init_usb_anchor(&dev->tx_submitted);
	atomic_set(&dev->active_tx_urbs, 0);
	spin_lock_init(&dev->tx_ctx_lock);
	for (rc = 0; rc < GS_MAX_TX_URBS; rc++) {
		dev->tx_context[rc].dev = dev;
		dev->tx_context[rc].echo_id = GS_MAX_TX_URBS;
	}

	/* can settup */
	dev->can.state = CAN_STATE_STOPPED;
	dev->can.clock.freq = bt_const->fclk_can;
	dev->can.bittiming_const = &dev->bt_const;
	dev->can.do_set_bittiming = gs_usb_set_bittiming;

	dev->can.ctrlmode_supported = 0;

	if (bt_const->feature & GS_CAN_FEATURE_LISTEN_ONLY)
		dev->can.ctrlmode_supported |= CAN_CTRLMODE_LISTENONLY;

	if (bt_const->feature & GS_CAN_FEATURE_LOOP_BACK)
		dev->can.ctrlmode_supported |= CAN_CTRLMODE_LOOPBACK;

	if (bt_const->feature & GS_CAN_FEATURE_TRIPLE_SAMPLE)
		dev->can.ctrlmode_supported |= CAN_CTRLMODE_3_SAMPLES;

	if (bt_const->feature & GS_CAN_FEATURE_ONE_SHOT)
		dev->can.ctrlmode_supported |= CAN_CTRLMODE_ONE_SHOT;

	SET_NETDEV_DEV(netdev, &intf->dev);

	if (dconf->sw_version > 1)
		if (bt_const->feature & GS_CAN_FEATURE_IDENTIFY)
			netdev->ethtool_ops = &gs_usb_ethtool_ops;

	kfree(bt_const);

	rc = register_candev(dev->netdev);
	if (rc) {
		free_candev(dev->netdev);
		dev_err(&intf->dev, "Couldn't register candev (err=%d)\n", rc);
		return ERR_PTR(rc);
	}

	return dev;
}
