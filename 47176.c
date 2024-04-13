static void oz_hcd_complete_set_config(struct oz_port *port, struct urb *urb,
		u8 rcode, u8 config_num)
{
	int rc = 0;
	struct usb_hcd *hcd = port->ozhcd->hcd;

	if (rcode == 0) {
		port->config_num = config_num;
		oz_clean_endpoints_for_config(hcd, port);
		if (oz_build_endpoints_for_config(hcd, port,
			&urb->dev->config[port->config_num-1], GFP_ATOMIC)) {
			rc = -ENOMEM;
		}
	} else {
		rc = -ENOMEM;
	}
	oz_complete_urb(hcd, urb, rc);
}
