static int ims_pcu_handle_firmware_update(struct ims_pcu *pcu,
					  const struct firmware *fw)
{
	unsigned int n_fw_records;
	int retval;

	dev_info(pcu->dev, "Updating firmware %s, size: %zu\n",
		 IMS_PCU_FIRMWARE_NAME, fw->size);

	n_fw_records = ims_pcu_count_fw_records(fw);

	retval = ims_pcu_flash_firmware(pcu, fw, n_fw_records);
	if (retval)
		goto out;

	retval = ims_pcu_execute_bl_command(pcu, LAUNCH_APP, NULL, 0, 0);
	if (retval)
		dev_err(pcu->dev,
			"Failed to start application image, error: %d\n",
			retval);

out:
	pcu->update_firmware_status = retval;
	sysfs_notify(&pcu->dev->kobj, NULL, "update_firmware_status");
	return retval;
}
