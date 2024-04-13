static int ims_pcu_send_cmd_chunk(struct ims_pcu *pcu,
				  u8 command, int chunk, int len)
{
	int error;

	error = usb_bulk_msg(pcu->udev,
			     usb_sndbulkpipe(pcu->udev,
					     pcu->ep_out->bEndpointAddress),
			     pcu->urb_out_buf, len,
			     NULL, IMS_PCU_CMD_WRITE_TIMEOUT);
	if (error < 0) {
		dev_dbg(pcu->dev,
			"Sending 0x%02x command failed at chunk %d: %d\n",
			command, chunk, error);
		return error;
	}

	return 0;
}
