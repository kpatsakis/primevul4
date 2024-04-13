static void powermate_disconnect(struct usb_interface *intf)
{
	struct powermate_device *pm = usb_get_intfdata (intf);

	usb_set_intfdata(intf, NULL);
	if (pm) {
		pm->requires_update = 0;
		usb_kill_urb(pm->irq);
		input_unregister_device(pm->input);
		usb_free_urb(pm->irq);
		usb_free_urb(pm->config);
		powermate_free_buffers(interface_to_usbdev(intf), pm);

		kfree(pm);
	}
}
