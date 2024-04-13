int usb_port_suspend(struct usb_device *udev, pm_message_t msg)
{
	struct usb_hub	*hub = usb_hub_to_struct_hub(udev->parent);
	struct usb_port *port_dev = hub->ports[udev->portnum - 1];
	int		port1 = udev->portnum;
	int		status;
	bool		really_suspend = true;

	usb_lock_port(port_dev);

	/* enable remote wakeup when appropriate; this lets the device
	 * wake up the upstream hub (including maybe the root hub).
	 *
	 * NOTE:  OTG devices may issue remote wakeup (or SRP) even when
	 * we don't explicitly enable it here.
	 */
	if (udev->do_remote_wakeup) {
		status = usb_enable_remote_wakeup(udev);
		if (status) {
			dev_dbg(&udev->dev, "won't remote wakeup, status %d\n",
					status);
			/* bail if autosuspend is requested */
			if (PMSG_IS_AUTO(msg))
				goto err_wakeup;
		}
	}

	/* disable USB2 hardware LPM */
	if (udev->usb2_hw_lpm_enabled == 1)
		usb_set_usb2_hardware_lpm(udev, 0);

	if (usb_disable_ltm(udev)) {
		dev_err(&udev->dev, "Failed to disable LTM before suspend\n.");
		status = -ENOMEM;
		if (PMSG_IS_AUTO(msg))
			goto err_ltm;
	}
	if (usb_unlocked_disable_lpm(udev)) {
		dev_err(&udev->dev, "Failed to disable LPM before suspend\n.");
		status = -ENOMEM;
		if (PMSG_IS_AUTO(msg))
			goto err_lpm3;
	}

	/* see 7.1.7.6 */
	if (hub_is_superspeed(hub->hdev))
		status = hub_set_port_link_state(hub, port1, USB_SS_PORT_LS_U3);

	/*
	 * For system suspend, we do not need to enable the suspend feature
	 * on individual USB-2 ports.  The devices will automatically go
	 * into suspend a few ms after the root hub stops sending packets.
	 * The USB 2.0 spec calls this "global suspend".
	 *
	 * However, many USB hubs have a bug: They don't relay wakeup requests
	 * from a downstream port if the port's suspend feature isn't on.
	 * Therefore we will turn on the suspend feature if udev or any of its
	 * descendants is enabled for remote wakeup.
	 */
	else if (PMSG_IS_AUTO(msg) || wakeup_enabled_descendants(udev) > 0)
		status = set_port_feature(hub->hdev, port1,
				USB_PORT_FEAT_SUSPEND);
	else {
		really_suspend = false;
		status = 0;
	}
	if (status) {
		dev_dbg(&port_dev->dev, "can't suspend, status %d\n", status);

		/* Try to enable USB3 LPM and LTM again */
		usb_unlocked_enable_lpm(udev);
 err_lpm3:
		usb_enable_ltm(udev);
 err_ltm:
		/* Try to enable USB2 hardware LPM again */
		if (udev->usb2_hw_lpm_capable == 1)
			usb_set_usb2_hardware_lpm(udev, 1);

		if (udev->do_remote_wakeup)
			(void) usb_disable_remote_wakeup(udev);
 err_wakeup:

		/* System sleep transitions should never fail */
		if (!PMSG_IS_AUTO(msg))
			status = 0;
	} else {
		dev_dbg(&udev->dev, "usb %ssuspend, wakeup %d\n",
				(PMSG_IS_AUTO(msg) ? "auto-" : ""),
				udev->do_remote_wakeup);
		if (really_suspend) {
			udev->port_is_suspended = 1;

			/* device has up to 10 msec to fully suspend */
			msleep(10);
		}
		usb_set_device_state(udev, USB_STATE_SUSPENDED);
	}

	if (status == 0 && !udev->do_remote_wakeup && udev->persist_enabled
			&& test_and_clear_bit(port1, hub->child_usage_bits))
		pm_runtime_put_sync(&port_dev->dev);

	usb_mark_last_busy(hub->hdev);

	usb_unlock_port(port_dev);
	return status;
}
