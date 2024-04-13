static int __ims_pcu_execute_bl_command(struct ims_pcu *pcu,
				        u8 command, const void *data, size_t len,
				        u8 expected_response, int response_time)
{
	int error;

	pcu->cmd_buf[0] = command;
	if (data)
		memcpy(&pcu->cmd_buf[1], data, len);

	error = __ims_pcu_execute_command(pcu,
				IMS_PCU_CMD_BOOTLOADER, pcu->cmd_buf, len + 1,
				expected_response ? IMS_PCU_RSP_BOOTLOADER : 0,
				response_time);
	if (error) {
		dev_err(pcu->dev,
			"Failure when sending 0x%02x command to bootloader, error: %d\n",
			pcu->cmd_buf[0], error);
		return error;
	}

	if (expected_response && pcu->cmd_buf[2] != expected_response) {
		dev_err(pcu->dev,
			"Unexpected response from bootloader: 0x%02x, wanted 0x%02x\n",
			pcu->cmd_buf[2], expected_response);
		return -EINVAL;
	}

	return 0;
}
