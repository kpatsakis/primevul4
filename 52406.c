static int ims_pcu_init_bootloader_mode(struct ims_pcu *pcu)
{
	int error;

	error = ims_pcu_execute_bl_command(pcu, QUERY_DEVICE, NULL, 0,
					   IMS_PCU_CMD_RESPONSE_TIMEOUT);
	if (error) {
		dev_err(pcu->dev, "Bootloader does not respond, aborting\n");
		return error;
	}

	pcu->fw_start_addr =
		get_unaligned_le32(&pcu->cmd_buf[IMS_PCU_DATA_OFFSET + 11]);
	pcu->fw_end_addr =
		get_unaligned_le32(&pcu->cmd_buf[IMS_PCU_DATA_OFFSET + 15]);

	dev_info(pcu->dev,
		 "Device is in bootloader mode (addr 0x%08x-0x%08x), requesting firmware\n",
		 pcu->fw_start_addr, pcu->fw_end_addr);

	error = request_firmware_nowait(THIS_MODULE, true,
					IMS_PCU_FIRMWARE_NAME,
					pcu->dev, GFP_KERNEL, pcu,
					ims_pcu_process_async_firmware);
	if (error) {
		/* This error is not fatal, let userspace have another chance */
		complete(&pcu->async_firmware_done);
	}

	return 0;
}
