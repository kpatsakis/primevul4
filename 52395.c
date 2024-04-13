static void ims_pcu_destroy_application_mode(struct ims_pcu *pcu)
{
	if (pcu->setup_complete) {
		pcu->setup_complete = false;
		mb(); /* make sure flag setting is not reordered */

		if (pcu->gamepad)
			ims_pcu_destroy_gamepad(pcu);
		ims_pcu_destroy_buttons(pcu);
		ims_pcu_destroy_backlight(pcu);

		if (pcu->device_id != IMS_PCU_PCU_B_DEVICE_ID)
			sysfs_remove_group(&pcu->dev->kobj,
					   &ims_pcu_ofn_attr_group);
	}
}
