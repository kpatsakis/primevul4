static void ims_pcu_backlight_work(struct work_struct *work)
{
	struct ims_pcu_backlight *backlight =
			container_of(work, struct ims_pcu_backlight, work);
	struct ims_pcu *pcu =
			container_of(backlight, struct ims_pcu, backlight);
	int desired_brightness = backlight->desired_brightness;
	__le16 br_val = cpu_to_le16(desired_brightness);
	int error;

	mutex_lock(&pcu->cmd_mutex);

	error = ims_pcu_execute_command(pcu, SET_BRIGHTNESS,
					&br_val, sizeof(br_val));
	if (error && error != -ENODEV)
		dev_warn(pcu->dev,
			 "Failed to set desired brightness %u, error: %d\n",
			 desired_brightness, error);

	mutex_unlock(&pcu->cmd_mutex);
}
