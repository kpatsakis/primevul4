void oz_hcd_control_cnf(void *hport, u8 req_id, u8 rcode, const u8 *data,
	int data_len)
{
	struct oz_port *port = hport;
	struct urb *urb;
	struct usb_ctrlrequest *setup;
	struct usb_hcd *hcd = port->ozhcd->hcd;
	unsigned windex;
	unsigned wvalue;

	oz_dbg(ON, "oz_hcd_control_cnf rcode=%u len=%d\n", rcode, data_len);
	urb = oz_find_urb_by_id(port, 0, req_id);
	if (!urb) {
		oz_dbg(ON, "URB not found\n");
		return;
	}
	setup = (struct usb_ctrlrequest *)urb->setup_packet;
	windex = le16_to_cpu(setup->wIndex);
	wvalue = le16_to_cpu(setup->wValue);
	if ((setup->bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
		/* Standard requests */
		oz_display_conf_type(setup->bRequest);
		switch (setup->bRequest) {
		case USB_REQ_SET_CONFIGURATION:
			oz_hcd_complete_set_config(port, urb, rcode,
				(u8)wvalue);
			break;
		case USB_REQ_SET_INTERFACE:
			oz_hcd_complete_set_interface(port, urb, rcode,
				(u8)windex, (u8)wvalue);
			break;
		default:
			oz_complete_urb(hcd, urb, 0);
		}

	} else {
		int copy_len;

		oz_dbg(ON, "VENDOR-CLASS - cnf\n");
		if (data_len) {
			if (data_len <= urb->transfer_buffer_length)
				copy_len = data_len;
			else
				copy_len = urb->transfer_buffer_length;
			memcpy(urb->transfer_buffer, data, copy_len);
			urb->actual_length = copy_len;
		}
		oz_complete_urb(hcd, urb, 0);
	}
}
