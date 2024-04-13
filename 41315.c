static void tg3_enable_ints(struct tg3 *tp)
{
	int i;

	tp->irq_sync = 0;
	wmb();

	tw32(TG3PCI_MISC_HOST_CTRL,
	     (tp->misc_host_ctrl & ~MISC_HOST_CTRL_MASK_PCI_INT));

	tp->coal_now = tp->coalesce_mode | HOSTCC_MODE_ENABLE;
	for (i = 0; i < tp->irq_cnt; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];

		tw32_mailbox_f(tnapi->int_mbox, tnapi->last_tag << 24);
		if (tg3_flag(tp, 1SHOT_MSI))
			tw32_mailbox_f(tnapi->int_mbox, tnapi->last_tag << 24);

		tp->coal_now |= tnapi->coal_now;
	}

	/* Force an initial interrupt */
	if (!tg3_flag(tp, TAGGED_STATUS) &&
	    (tp->napi[0].hw_status->status & SD_STATUS_UPDATED))
		tw32(GRC_LOCAL_CTRL, tp->grc_local_ctrl | GRC_LCLCTRL_SETINT);
	else
		tw32(HOSTCC_MODE, tp->coal_now);

	tp->coal_now &= ~(tp->napi[0].coal_now | tp->napi[1].coal_now);
}
