static void gtco_disconnect(struct usb_interface *interface)
{
	/* Grab private device ptr */
	struct gtco *gtco = usb_get_intfdata(interface);
	struct usb_device *udev = interface_to_usbdev(interface);

	/* Now reverse all the registration stuff */
	if (gtco) {
		input_unregister_device(gtco->inputdevice);
		usb_kill_urb(gtco->urbinfo);
		usb_free_urb(gtco->urbinfo);
		usb_free_coherent(udev, REPORT_MAX_SIZE,
				  gtco->buffer, gtco->buf_dma);
		kfree(gtco);
	}

	dev_info(&interface->dev, "gtco driver disconnected\n");
}
