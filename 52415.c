static void ims_pcu_process_async_firmware(const struct firmware *fw,
					   void *context)
{
	struct ims_pcu *pcu = context;
	int error;

	if (!fw) {
		dev_err(pcu->dev, "Failed to get firmware %s\n",
			IMS_PCU_FIRMWARE_NAME);
		goto out;
	}

	error = ihex_validate_fw(fw);
	if (error) {
		dev_err(pcu->dev, "Firmware %s is invalid\n",
			IMS_PCU_FIRMWARE_NAME);
		goto out;
	}

	mutex_lock(&pcu->cmd_mutex);
	ims_pcu_handle_firmware_update(pcu, fw);
	mutex_unlock(&pcu->cmd_mutex);

	release_firmware(fw);

out:
	complete(&pcu->async_firmware_done);
}
