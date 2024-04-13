static int tg3_test_interrupt(struct tg3 *tp)
{
	struct tg3_napi *tnapi = &tp->napi[0];
	struct net_device *dev = tp->dev;
	int err, i, intr_ok = 0;
	u32 val;

	if (!netif_running(dev))
		return -ENODEV;

	tg3_disable_ints(tp);

	free_irq(tnapi->irq_vec, tnapi);

	/*
	 * Turn off MSI one shot mode.  Otherwise this test has no
	 * observable way to know whether the interrupt was delivered.
	 */
	if (tg3_flag(tp, 57765_PLUS)) {
		val = tr32(MSGINT_MODE) | MSGINT_MODE_ONE_SHOT_DISABLE;
		tw32(MSGINT_MODE, val);
	}

	err = request_irq(tnapi->irq_vec, tg3_test_isr,
			  IRQF_SHARED, dev->name, tnapi);
	if (err)
		return err;

	tnapi->hw_status->status &= ~SD_STATUS_UPDATED;
	tg3_enable_ints(tp);

	tw32_f(HOSTCC_MODE, tp->coalesce_mode | HOSTCC_MODE_ENABLE |
	       tnapi->coal_now);

	for (i = 0; i < 5; i++) {
		u32 int_mbox, misc_host_ctrl;

		int_mbox = tr32_mailbox(tnapi->int_mbox);
		misc_host_ctrl = tr32(TG3PCI_MISC_HOST_CTRL);

		if ((int_mbox != 0) ||
		    (misc_host_ctrl & MISC_HOST_CTRL_MASK_PCI_INT)) {
			intr_ok = 1;
			break;
		}

		if (tg3_flag(tp, 57765_PLUS) &&
		    tnapi->hw_status->status_tag != tnapi->last_tag)
			tw32_mailbox_f(tnapi->int_mbox, tnapi->last_tag << 24);

		msleep(10);
	}

	tg3_disable_ints(tp);

	free_irq(tnapi->irq_vec, tnapi);

	err = tg3_request_irq(tp, 0);

	if (err)
		return err;

	if (intr_ok) {
		/* Reenable MSI one shot mode. */
		if (tg3_flag(tp, 57765_PLUS) && tg3_flag(tp, 1SHOT_MSI)) {
			val = tr32(MSGINT_MODE) & ~MSGINT_MODE_ONE_SHOT_DISABLE;
			tw32(MSGINT_MODE, val);
		}
		return 0;
	}

	return -EIO;
}
