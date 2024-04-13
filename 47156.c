static int oz_build_endpoints_for_interface(struct usb_hcd *hcd,
		struct oz_port *port,
		struct usb_host_interface *intf, gfp_t mem_flags)
{
	struct oz_hcd *ozhcd = port->ozhcd;
	int i;
	int if_ix = intf->desc.bInterfaceNumber;
	int request_heartbeat = 0;

	oz_dbg(ON, "interface[%d] = %p\n", if_ix, intf);
	if (if_ix >= port->num_iface || port->iface == NULL)
		return -ENOMEM;
	for (i = 0; i < intf->desc.bNumEndpoints; i++) {
		struct usb_host_endpoint *hep = &intf->endpoint[i];
		u8 ep_addr = hep->desc.bEndpointAddress;
		u8 ep_num = ep_addr & USB_ENDPOINT_NUMBER_MASK;
		struct oz_endpoint *ep;
		int buffer_size = 0;

		oz_dbg(ON, "%d bEndpointAddress = %x\n", i, ep_addr);
		if (ep_addr & USB_ENDPOINT_DIR_MASK) {
			switch (hep->desc.bmAttributes &
						USB_ENDPOINT_XFERTYPE_MASK) {
			case USB_ENDPOINT_XFER_ISOC:
				buffer_size = OZ_EP_BUFFER_SIZE_ISOC;
				break;
			case USB_ENDPOINT_XFER_INT:
				buffer_size = OZ_EP_BUFFER_SIZE_INT;
				break;
			}
		}

		ep = oz_ep_alloc(buffer_size, mem_flags);
		if (!ep) {
			oz_clean_endpoints_for_interface(hcd, port, if_ix);
			return -ENOMEM;
		}
		ep->attrib = hep->desc.bmAttributes;
		ep->ep_num = ep_num;
		if ((ep->attrib & USB_ENDPOINT_XFERTYPE_MASK)
			== USB_ENDPOINT_XFER_ISOC) {
			oz_dbg(ON, "wMaxPacketSize = %d\n",
			       usb_endpoint_maxp(&hep->desc));
			ep->credit_ceiling = 200;
			if (ep_addr & USB_ENDPOINT_DIR_MASK) {
				ep->flags |= OZ_F_EP_BUFFERING;
			} else {
				ep->flags |= OZ_F_EP_HAVE_STREAM;
				if (oz_usb_stream_create(port->hpd, ep_num))
					ep->flags &= ~OZ_F_EP_HAVE_STREAM;
			}
		}
		spin_lock_bh(&ozhcd->hcd_lock);
		if (ep_addr & USB_ENDPOINT_DIR_MASK) {
			port->in_ep[ep_num] = ep;
			port->iface[if_ix].ep_mask |=
				(1<<(ep_num+OZ_NB_ENDPOINTS));
			if ((ep->attrib & USB_ENDPOINT_XFERTYPE_MASK)
				 == USB_ENDPOINT_XFER_ISOC) {
				list_add_tail(&ep->link, &port->isoc_in_ep);
				request_heartbeat = 1;
			}
		} else {
			port->out_ep[ep_num] = ep;
			port->iface[if_ix].ep_mask |= (1<<ep_num);
			if ((ep->attrib & USB_ENDPOINT_XFERTYPE_MASK)
				== USB_ENDPOINT_XFER_ISOC) {
				list_add_tail(&ep->link, &port->isoc_out_ep);
				request_heartbeat = 1;
			}
		}
		spin_unlock_bh(&ozhcd->hcd_lock);
		if (request_heartbeat && port->hpd)
			oz_usb_request_heartbeat(port->hpd);
	}
	return 0;
}
