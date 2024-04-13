static void ims_pcu_destroy_backlight(struct ims_pcu *pcu)
{
	struct ims_pcu_backlight *backlight = &pcu->backlight;

	led_classdev_unregister(&backlight->cdev);
	cancel_work_sync(&backlight->work);
}
