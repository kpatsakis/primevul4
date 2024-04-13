static int ims_pcu_read_ofn_config(struct ims_pcu *pcu, u8 addr, u8 *data)
{
	int error;
	s16 result;

	error = ims_pcu_execute_command(pcu, OFN_GET_CONFIG,
					&addr, sizeof(addr));
	if (error)
		return error;

	result = (s16)get_unaligned_le16(pcu->cmd_buf + OFN_REG_RESULT_OFFSET);
	if (result < 0)
		return -EIO;

	/* We only need LSB */
	*data = pcu->cmd_buf[OFN_REG_RESULT_OFFSET];
	return 0;
}
