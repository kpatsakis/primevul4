static int ims_pcu_send_command(struct ims_pcu *pcu,
				u8 command, const u8 *data, int len)
{
	int count = 0;
	int chunk = 0;
	int delta;
	int i;
	int error;
	u8 csum = 0;
	u8 ack_id;

	pcu->urb_out_buf[count++] = IMS_PCU_PROTOCOL_STX;

	/* We know the command need not be escaped */
	pcu->urb_out_buf[count++] = command;
	csum += command;

	ack_id = pcu->ack_id++;
	if (ack_id == 0xff)
		ack_id = pcu->ack_id++;

	if (ims_pcu_byte_needs_escape(ack_id))
		pcu->urb_out_buf[count++] = IMS_PCU_PROTOCOL_DLE;

	pcu->urb_out_buf[count++] = ack_id;
	csum += ack_id;

	for (i = 0; i < len; i++) {

		delta = ims_pcu_byte_needs_escape(data[i]) ? 2 : 1;
		if (count + delta >= pcu->max_out_size) {
			error = ims_pcu_send_cmd_chunk(pcu, command,
						       ++chunk, count);
			if (error)
				return error;

			count = 0;
		}

		if (delta == 2)
			pcu->urb_out_buf[count++] = IMS_PCU_PROTOCOL_DLE;

		pcu->urb_out_buf[count++] = data[i];
		csum += data[i];
	}

	csum = 1 + ~csum;

	delta = ims_pcu_byte_needs_escape(csum) ? 3 : 2;
	if (count + delta >= pcu->max_out_size) {
		error = ims_pcu_send_cmd_chunk(pcu, command, ++chunk, count);
		if (error)
			return error;

		count = 0;
	}

	if (delta == 3)
		pcu->urb_out_buf[count++] = IMS_PCU_PROTOCOL_DLE;

	pcu->urb_out_buf[count++] = csum;
	pcu->urb_out_buf[count++] = IMS_PCU_PROTOCOL_ETX;

	return ims_pcu_send_cmd_chunk(pcu, command, ++chunk, count);
}
