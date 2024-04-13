static void oz_hcd_complete_set_interface(struct oz_port *port, struct urb *urb,
		u8 rcode, u8 if_num, u8 alt)
{
	struct usb_hcd *hcd = port->ozhcd->hcd;
	int rc = 0;

	if ((rcode == 0) && (port->config_num > 0)) {
		struct usb_host_config *config;
		struct usb_host_interface *intf;

		oz_dbg(ON, "Set interface %d alt %d\n", if_num, alt);
		oz_clean_endpoints_for_interface(hcd, port, if_num);
		config = &urb->dev->config[port->config_num-1];
		intf = &config->intf_cache[if_num]->altsetting[alt];
		if (oz_build_endpoints_for_interface(hcd, port, intf,
			GFP_ATOMIC))
			rc = -ENOMEM;
		else
			port->iface[if_num].alt = alt;
	} else {
		rc = -ENOMEM;
	}
	oz_complete_urb(hcd, urb, rc);
}
