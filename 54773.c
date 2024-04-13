static void sg_timeout(unsigned long _req)
{
	struct usb_sg_request	*req = (struct usb_sg_request *) _req;

	usb_sg_cancel(req);
}
