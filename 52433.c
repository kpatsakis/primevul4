static int ims_pcu_write_ofn_config(struct ims_pcu *pcu, u8 addr, u8 data)
{
	u8 buffer[] = { addr, data };
	int error;
	s16 result;

	error = ims_pcu_execute_command(pcu, OFN_SET_CONFIG,
					&buffer, sizeof(buffer));
	if (error)
		return error;

	result = (s16)get_unaligned_le16(pcu->cmd_buf + OFN_REG_RESULT_OFFSET);
	if (result < 0)
		return -EIO;

	return 0;
}
