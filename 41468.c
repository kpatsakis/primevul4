static inline int tg3_pwrsrc_switch_to_vmain(struct tg3 *tp)
{
	if (!tg3_flag(tp, IS_NIC))
		return 0;

	if (tg3_asic_rev(tp) == ASIC_REV_5717 ||
	    tg3_asic_rev(tp) == ASIC_REV_5719 ||
	    tg3_asic_rev(tp) == ASIC_REV_5720) {
		if (tg3_ape_lock(tp, TG3_APE_LOCK_GPIO))
			return -EIO;

		tg3_set_function_status(tp, TG3_GPIO_MSG_DRVR_PRES);

		tw32_wait_f(GRC_LOCAL_CTRL, tp->grc_local_ctrl,
			    TG3_GRC_LCLCTL_PWRSW_DELAY);

		tg3_ape_unlock(tp, TG3_APE_LOCK_GPIO);
	} else {
		tw32_wait_f(GRC_LOCAL_CTRL, tp->grc_local_ctrl,
			    TG3_GRC_LCLCTL_PWRSW_DELAY);
	}

	return 0;
}
