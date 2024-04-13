void usbip_dump_header(struct usbip_header *pdu)
{
	pr_debug("BASE: cmd %u seq %u devid %u dir %u ep %u\n",
		 pdu->base.command,
		 pdu->base.seqnum,
		 pdu->base.devid,
		 pdu->base.direction,
		 pdu->base.ep);

	switch (pdu->base.command) {
	case USBIP_CMD_SUBMIT:
		pr_debug("USBIP_CMD_SUBMIT: x_flags %u x_len %u sf %u #p %d iv %d\n",
			 pdu->u.cmd_submit.transfer_flags,
			 pdu->u.cmd_submit.transfer_buffer_length,
			 pdu->u.cmd_submit.start_frame,
			 pdu->u.cmd_submit.number_of_packets,
			 pdu->u.cmd_submit.interval);
		break;
	case USBIP_CMD_UNLINK:
		pr_debug("USBIP_CMD_UNLINK: seq %u\n",
			 pdu->u.cmd_unlink.seqnum);
		break;
	case USBIP_RET_SUBMIT:
		pr_debug("USBIP_RET_SUBMIT: st %d al %u sf %d #p %d ec %d\n",
			 pdu->u.ret_submit.status,
			 pdu->u.ret_submit.actual_length,
			 pdu->u.ret_submit.start_frame,
			 pdu->u.ret_submit.number_of_packets,
			 pdu->u.ret_submit.error_count);
		break;
	case USBIP_RET_UNLINK:
		pr_debug("USBIP_RET_UNLINK: status %d\n",
			 pdu->u.ret_unlink.status);
		break;
	default:
		/* NOT REACHED */
		pr_err("unknown command\n");
		break;
	}
}
