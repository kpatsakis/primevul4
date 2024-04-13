static unsigned get_maxpacket(struct usb_device *udev, int pipe)
{
	struct usb_host_endpoint	*ep;

	ep = usb_pipe_endpoint(udev, pipe);
	return le16_to_cpup(&ep->desc.wMaxPacketSize);
}
