static void tg3_frob_aux_power_5717(struct tg3 *tp, bool wol_enable)
{
	u32 msg = 0;

	/* Serialize power state transitions */
	if (tg3_ape_lock(tp, TG3_APE_LOCK_GPIO))
		return;

	if (tg3_flag(tp, ENABLE_ASF) || tg3_flag(tp, ENABLE_APE) || wol_enable)
		msg = TG3_GPIO_MSG_NEED_VAUX;

	msg = tg3_set_function_status(tp, msg);

	if (msg & TG3_GPIO_MSG_ALL_DRVR_PRES_MASK)
		goto done;

	if (msg & TG3_GPIO_MSG_ALL_NEED_VAUX_MASK)
		tg3_pwrsrc_switch_to_vaux(tp);
	else
		tg3_pwrsrc_die_with_vmain(tp);

done:
	tg3_ape_unlock(tp, TG3_APE_LOCK_GPIO);
}
