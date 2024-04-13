static void driver_disconnect(struct usb_interface *intf)
{
	struct usb_dev_state *ps = usb_get_intfdata(intf);
	unsigned int ifnum = intf->altsetting->desc.bInterfaceNumber;

	if (!ps)
		return;

	/* NOTE:  this relies on usbcore having canceled and completed
	 * all pending I/O requests; 2.6 does that.
	 */

	if (likely(ifnum < 8*sizeof(ps->ifclaimed)))
		clear_bit(ifnum, &ps->ifclaimed);
	else
		dev_warn(&intf->dev, "interface number %u out of range\n",
			 ifnum);

	usb_set_intfdata(intf, NULL);

	/* force async requests to complete */
	destroy_async_on_interface(ps, ifnum);
}
