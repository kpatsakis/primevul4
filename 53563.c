static void hub_port_connect(struct usb_hub *hub, int port1, u16 portstatus,
		u16 portchange)
{
	int status, i;
	unsigned unit_load;
	struct usb_device *hdev = hub->hdev;
	struct usb_hcd *hcd = bus_to_hcd(hdev->bus);
	struct usb_port *port_dev = hub->ports[port1 - 1];
	struct usb_device *udev = port_dev->child;
	static int unreliable_port = -1;

	/* Disconnect any existing devices under this port */
	if (udev) {
		if (hcd->usb_phy && !hdev->parent)
			usb_phy_notify_disconnect(hcd->usb_phy, udev->speed);
		usb_disconnect(&port_dev->child);
	}

	/* We can forget about a "removed" device when there's a physical
	 * disconnect or the connect status changes.
	 */
	if (!(portstatus & USB_PORT_STAT_CONNECTION) ||
			(portchange & USB_PORT_STAT_C_CONNECTION))
		clear_bit(port1, hub->removed_bits);

	if (portchange & (USB_PORT_STAT_C_CONNECTION |
				USB_PORT_STAT_C_ENABLE)) {
		status = hub_port_debounce_be_stable(hub, port1);
		if (status < 0) {
			if (status != -ENODEV &&
				port1 != unreliable_port &&
				printk_ratelimit())
				dev_err(&port_dev->dev, "connect-debounce failed\n");
			portstatus &= ~USB_PORT_STAT_CONNECTION;
			unreliable_port = port1;
		} else {
			portstatus = status;
		}
	}

	/* Return now if debouncing failed or nothing is connected or
	 * the device was "removed".
	 */
	if (!(portstatus & USB_PORT_STAT_CONNECTION) ||
			test_bit(port1, hub->removed_bits)) {

		/*
		 * maybe switch power back on (e.g. root hub was reset)
		 * but only if the port isn't owned by someone else.
		 */
		if (hub_is_port_power_switchable(hub)
				&& !port_is_power_on(hub, portstatus)
				&& !port_dev->port_owner)
			set_port_feature(hdev, port1, USB_PORT_FEAT_POWER);

		if (portstatus & USB_PORT_STAT_ENABLE)
			goto done;
		return;
	}
	if (hub_is_superspeed(hub->hdev))
		unit_load = 150;
	else
		unit_load = 100;

	status = 0;
	for (i = 0; i < SET_CONFIG_TRIES; i++) {

		/* reallocate for each attempt, since references
		 * to the previous one can escape in various ways
		 */
		udev = usb_alloc_dev(hdev, hdev->bus, port1);
		if (!udev) {
			dev_err(&port_dev->dev,
					"couldn't allocate usb_device\n");
			goto done;
		}

		usb_set_device_state(udev, USB_STATE_POWERED);
		udev->bus_mA = hub->mA_per_port;
		udev->level = hdev->level + 1;
		udev->wusb = hub_is_wusb(hub);

		/* Only USB 3.0 devices are connected to SuperSpeed hubs. */
		if (hub_is_superspeed(hub->hdev))
			udev->speed = USB_SPEED_SUPER;
		else
			udev->speed = USB_SPEED_UNKNOWN;

		choose_devnum(udev);
		if (udev->devnum <= 0) {
			status = -ENOTCONN;	/* Don't retry */
			goto loop;
		}

		/* reset (non-USB 3.0 devices) and get descriptor */
		usb_lock_port(port_dev);
		status = hub_port_init(hub, udev, port1, i);
		usb_unlock_port(port_dev);
		if (status < 0)
			goto loop;

		if (udev->quirks & USB_QUIRK_DELAY_INIT)
			msleep(1000);

		/* consecutive bus-powered hubs aren't reliable; they can
		 * violate the voltage drop budget.  if the new child has
		 * a "powered" LED, users should notice we didn't enable it
		 * (without reading syslog), even without per-port LEDs
		 * on the parent.
		 */
		if (udev->descriptor.bDeviceClass == USB_CLASS_HUB
				&& udev->bus_mA <= unit_load) {
			u16	devstat;

			status = usb_get_status(udev, USB_RECIP_DEVICE, 0,
					&devstat);
			if (status) {
				dev_dbg(&udev->dev, "get status %d ?\n", status);
				goto loop_disable;
			}
			if ((devstat & (1 << USB_DEVICE_SELF_POWERED)) == 0) {
				dev_err(&udev->dev,
					"can't connect bus-powered hub "
					"to this port\n");
				if (hub->has_indicators) {
					hub->indicator[port1-1] =
						INDICATOR_AMBER_BLINK;
					queue_delayed_work(
						system_power_efficient_wq,
						&hub->leds, 0);
				}
				status = -ENOTCONN;	/* Don't retry */
				goto loop_disable;
			}
		}

		/* check for devices running slower than they could */
		if (le16_to_cpu(udev->descriptor.bcdUSB) >= 0x0200
				&& udev->speed == USB_SPEED_FULL
				&& highspeed_hubs != 0)
			check_highspeed(hub, udev, port1);

		/* Store the parent's children[] pointer.  At this point
		 * udev becomes globally accessible, although presumably
		 * no one will look at it until hdev is unlocked.
		 */
		status = 0;

		mutex_lock(&usb_port_peer_mutex);

		/* We mustn't add new devices if the parent hub has
		 * been disconnected; we would race with the
		 * recursively_mark_NOTATTACHED() routine.
		 */
		spin_lock_irq(&device_state_lock);
		if (hdev->state == USB_STATE_NOTATTACHED)
			status = -ENOTCONN;
		else
			port_dev->child = udev;
		spin_unlock_irq(&device_state_lock);
		mutex_unlock(&usb_port_peer_mutex);

		/* Run it through the hoops (find a driver, etc) */
		if (!status) {
			status = usb_new_device(udev);
			if (status) {
				mutex_lock(&usb_port_peer_mutex);
				spin_lock_irq(&device_state_lock);
				port_dev->child = NULL;
				spin_unlock_irq(&device_state_lock);
				mutex_unlock(&usb_port_peer_mutex);
			} else {
				if (hcd->usb_phy && !hdev->parent)
					usb_phy_notify_connect(hcd->usb_phy,
							udev->speed);
			}
		}

		if (status)
			goto loop_disable;

		status = hub_power_remaining(hub);
		if (status)
			dev_dbg(hub->intfdev, "%dmA power budget left\n", status);

		return;

loop_disable:
		hub_port_disable(hub, port1, 1);
loop:
		usb_ep0_reinit(udev);
		release_devnum(udev);
		hub_free_dev(udev);
		usb_put_dev(udev);
		if ((status == -ENOTCONN) || (status == -ENOTSUPP))
			break;
	}
	if (hub->hdev->parent ||
			!hcd->driver->port_handed_over ||
			!(hcd->driver->port_handed_over)(hcd, port1)) {
		if (status != -ENOTCONN && status != -ENODEV)
			dev_err(&port_dev->dev,
					"unable to enumerate USB device\n");
	}

done:
	hub_port_disable(hub, port1, 1);
	if (hcd->driver->relinquish_port && !hub->hdev->parent)
		hcd->driver->relinquish_port(hcd, port1);

}
