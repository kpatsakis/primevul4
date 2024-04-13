void usbip_header_correct_endian(struct usbip_header *pdu, int send)
{
	__u32 cmd = 0;

	if (send)
		cmd = pdu->base.command;

	correct_endian_basic(&pdu->base, send);

	if (!send)
		cmd = pdu->base.command;

	switch (cmd) {
	case USBIP_CMD_SUBMIT:
		correct_endian_cmd_submit(&pdu->u.cmd_submit, send);
		break;
	case USBIP_RET_SUBMIT:
		correct_endian_ret_submit(&pdu->u.ret_submit, send);
		break;
	case USBIP_CMD_UNLINK:
		correct_endian_cmd_unlink(&pdu->u.cmd_unlink, send);
		break;
	case USBIP_RET_UNLINK:
		correct_endian_ret_unlink(&pdu->u.ret_unlink, send);
		break;
	default:
		/* NOT REACHED */
		pr_err("unknown command\n");
		break;
	}
}
