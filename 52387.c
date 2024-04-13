ims_pcu_backlight_get_brightness(struct led_classdev *cdev)
{
	struct ims_pcu_backlight *backlight =
			container_of(cdev, struct ims_pcu_backlight, cdev);
	struct ims_pcu *pcu =
			container_of(backlight, struct ims_pcu, backlight);
	int brightness;
	int error;

	mutex_lock(&pcu->cmd_mutex);

	error = ims_pcu_execute_query(pcu, GET_BRIGHTNESS);
	if (error) {
		dev_warn(pcu->dev,
			 "Failed to get current brightness, error: %d\n",
			 error);
		/* Assume the LED is OFF */
		brightness = LED_OFF;
	} else {
		brightness =
			get_unaligned_le16(&pcu->cmd_buf[IMS_PCU_DATA_OFFSET]);
	}

	mutex_unlock(&pcu->cmd_mutex);

	return brightness;
}
