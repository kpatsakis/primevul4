static void ims_pcu_backlight_set_brightness(struct led_classdev *cdev,
					     enum led_brightness value)
{
	struct ims_pcu_backlight *backlight =
			container_of(cdev, struct ims_pcu_backlight, cdev);

	backlight->desired_brightness = value;
	schedule_work(&backlight->work);
}
