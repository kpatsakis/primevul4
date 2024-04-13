static void ims_pcu_report_events(struct ims_pcu *pcu)
{
	u32 data = get_unaligned_be32(&pcu->read_buf[3]);

	ims_pcu_buttons_report(pcu, data & ~IMS_PCU_GAMEPAD_MASK);
	if (pcu->gamepad)
		ims_pcu_gamepad_report(pcu, data);
}
