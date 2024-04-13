void usbip_pack_pdu(struct usbip_header *pdu, struct urb *urb, int cmd,
		    int pack)
{
	switch (cmd) {
	case USBIP_CMD_SUBMIT:
		usbip_pack_cmd_submit(pdu, urb, pack);
		break;
	case USBIP_RET_SUBMIT:
		usbip_pack_ret_submit(pdu, urb, pack);
		break;
	default:
		/* NOT REACHED */
		pr_err("unknown command\n");
		break;
	}
}
