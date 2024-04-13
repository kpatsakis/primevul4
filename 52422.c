static int ims_pcu_set_info(struct ims_pcu *pcu)
{
	int error;

	memcpy(&pcu->cmd_buf[IMS_PCU_INFO_PART_OFFSET],
	       pcu->part_number, sizeof(pcu->part_number));
	memcpy(&pcu->cmd_buf[IMS_PCU_INFO_DOM_OFFSET],
	       pcu->date_of_manufacturing, sizeof(pcu->date_of_manufacturing));
	memcpy(&pcu->cmd_buf[IMS_PCU_INFO_SERIAL_OFFSET],
	       pcu->serial_number, sizeof(pcu->serial_number));

	error = ims_pcu_execute_command(pcu, SET_INFO,
					&pcu->cmd_buf[IMS_PCU_DATA_OFFSET],
					IMS_PCU_SET_INFO_SIZE);
	if (error) {
		dev_err(pcu->dev,
			"Failed to update device information, error: %d\n",
			error);
		return error;
	}

	return 0;
}
