static int ims_pcu_get_device_info(struct ims_pcu *pcu)
{
	int error;

	error = ims_pcu_get_info(pcu);
	if (error)
		return error;

	error = ims_pcu_execute_query(pcu, GET_FW_VERSION);
	if (error) {
		dev_err(pcu->dev,
			"GET_FW_VERSION command failed, error: %d\n", error);
		return error;
	}

	snprintf(pcu->fw_version, sizeof(pcu->fw_version),
		 "%02d%02d%02d%02d.%c%c",
		 pcu->cmd_buf[2], pcu->cmd_buf[3], pcu->cmd_buf[4], pcu->cmd_buf[5],
		 pcu->cmd_buf[6], pcu->cmd_buf[7]);

	error = ims_pcu_execute_query(pcu, GET_BL_VERSION);
	if (error) {
		dev_err(pcu->dev,
			"GET_BL_VERSION command failed, error: %d\n", error);
		return error;
	}

	snprintf(pcu->bl_version, sizeof(pcu->bl_version),
		 "%02d%02d%02d%02d.%c%c",
		 pcu->cmd_buf[2], pcu->cmd_buf[3], pcu->cmd_buf[4], pcu->cmd_buf[5],
		 pcu->cmd_buf[6], pcu->cmd_buf[7]);

	error = ims_pcu_execute_query(pcu, RESET_REASON);
	if (error) {
		dev_err(pcu->dev,
			"RESET_REASON command failed, error: %d\n", error);
		return error;
	}

	snprintf(pcu->reset_reason, sizeof(pcu->reset_reason),
		 "%02x", pcu->cmd_buf[IMS_PCU_DATA_OFFSET]);

	dev_dbg(pcu->dev,
		"P/N: %s, MD: %s, S/N: %s, FW: %s, BL: %s, RR: %s\n",
		pcu->part_number,
		pcu->date_of_manufacturing,
		pcu->serial_number,
		pcu->fw_version,
		pcu->bl_version,
		pcu->reset_reason);

	return 0;
}
