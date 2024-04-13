static int oz_clear_port_feature(struct usb_hcd *hcd, u16 wvalue, u16 windex)
{
	struct oz_port *port;
	u8 port_id = (u8)windex;
	struct oz_hcd *ozhcd = oz_hcd_private(hcd);
	unsigned clear_bits = 0;

	if ((port_id < 1) || (port_id > OZ_NB_PORTS))
		return -EPIPE;
	port = &ozhcd->ports[port_id-1];
	switch (wvalue) {
	case USB_PORT_FEAT_CONNECTION:
		oz_dbg(HUB, "USB_PORT_FEAT_CONNECTION\n");
		break;
	case USB_PORT_FEAT_ENABLE:
		oz_dbg(HUB, "USB_PORT_FEAT_ENABLE\n");
		clear_bits = USB_PORT_STAT_ENABLE;
		break;
	case USB_PORT_FEAT_SUSPEND:
		oz_dbg(HUB, "USB_PORT_FEAT_SUSPEND\n");
		break;
	case USB_PORT_FEAT_OVER_CURRENT:
		oz_dbg(HUB, "USB_PORT_FEAT_OVER_CURRENT\n");
		break;
	case USB_PORT_FEAT_RESET:
		oz_dbg(HUB, "USB_PORT_FEAT_RESET\n");
		break;
	case USB_PORT_FEAT_POWER:
		oz_dbg(HUB, "USB_PORT_FEAT_POWER\n");
		clear_bits |= USB_PORT_STAT_POWER;
		break;
	case USB_PORT_FEAT_LOWSPEED:
		oz_dbg(HUB, "USB_PORT_FEAT_LOWSPEED\n");
		break;
	case USB_PORT_FEAT_C_CONNECTION:
		oz_dbg(HUB, "USB_PORT_FEAT_C_CONNECTION\n");
		clear_bits = USB_PORT_STAT_C_CONNECTION << 16;
		break;
	case USB_PORT_FEAT_C_ENABLE:
		oz_dbg(HUB, "USB_PORT_FEAT_C_ENABLE\n");
		clear_bits = USB_PORT_STAT_C_ENABLE << 16;
		break;
	case USB_PORT_FEAT_C_SUSPEND:
		oz_dbg(HUB, "USB_PORT_FEAT_C_SUSPEND\n");
		break;
	case USB_PORT_FEAT_C_OVER_CURRENT:
		oz_dbg(HUB, "USB_PORT_FEAT_C_OVER_CURRENT\n");
		break;
	case USB_PORT_FEAT_C_RESET:
		oz_dbg(HUB, "USB_PORT_FEAT_C_RESET\n");
		clear_bits = USB_PORT_FEAT_C_RESET << 16;
		break;
	case USB_PORT_FEAT_TEST:
		oz_dbg(HUB, "USB_PORT_FEAT_TEST\n");
		break;
	case USB_PORT_FEAT_INDICATOR:
		oz_dbg(HUB, "USB_PORT_FEAT_INDICATOR\n");
		break;
	default:
		oz_dbg(HUB, "Other %d\n", wvalue);
		break;
	}
	if (clear_bits) {
		spin_lock_bh(&port->port_lock);
		port->status &= ~clear_bits;
		spin_unlock_bh(&port->port_lock);
	}
	oz_dbg(HUB, "Port[%d] status = 0x%x\n",
	       port_id, ozhcd->ports[port_id-1].status);
	return 0;
}
