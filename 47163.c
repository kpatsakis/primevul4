static void oz_display_conf_type(u8 t)
{
	switch (t) {
	case USB_REQ_GET_STATUS:
		oz_dbg(ON, "USB_REQ_GET_STATUS - cnf\n");
		break;
	case USB_REQ_CLEAR_FEATURE:
		oz_dbg(ON, "USB_REQ_CLEAR_FEATURE - cnf\n");
		break;
	case USB_REQ_SET_FEATURE:
		oz_dbg(ON, "USB_REQ_SET_FEATURE - cnf\n");
		break;
	case USB_REQ_SET_ADDRESS:
		oz_dbg(ON, "USB_REQ_SET_ADDRESS - cnf\n");
		break;
	case USB_REQ_GET_DESCRIPTOR:
		oz_dbg(ON, "USB_REQ_GET_DESCRIPTOR - cnf\n");
		break;
	case USB_REQ_SET_DESCRIPTOR:
		oz_dbg(ON, "USB_REQ_SET_DESCRIPTOR - cnf\n");
		break;
	case USB_REQ_GET_CONFIGURATION:
		oz_dbg(ON, "USB_REQ_GET_CONFIGURATION - cnf\n");
		break;
	case USB_REQ_SET_CONFIGURATION:
		oz_dbg(ON, "USB_REQ_SET_CONFIGURATION - cnf\n");
		break;
	case USB_REQ_GET_INTERFACE:
		oz_dbg(ON, "USB_REQ_GET_INTERFACE - cnf\n");
		break;
	case USB_REQ_SET_INTERFACE:
		oz_dbg(ON, "USB_REQ_SET_INTERFACE - cnf\n");
		break;
	case USB_REQ_SYNCH_FRAME:
		oz_dbg(ON, "USB_REQ_SYNCH_FRAME - cnf\n");
		break;
	}
}
