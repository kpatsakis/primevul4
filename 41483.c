static int tg3_request_irq(struct tg3 *tp, int irq_num)
{
	irq_handler_t fn;
	unsigned long flags;
	char *name;
	struct tg3_napi *tnapi = &tp->napi[irq_num];

	if (tp->irq_cnt == 1)
		name = tp->dev->name;
	else {
		name = &tnapi->irq_lbl[0];
		snprintf(name, IFNAMSIZ, "%s-%d", tp->dev->name, irq_num);
		name[IFNAMSIZ-1] = 0;
	}

	if (tg3_flag(tp, USING_MSI) || tg3_flag(tp, USING_MSIX)) {
		fn = tg3_msi;
		if (tg3_flag(tp, 1SHOT_MSI))
			fn = tg3_msi_1shot;
		flags = 0;
	} else {
		fn = tg3_interrupt;
		if (tg3_flag(tp, TAGGED_STATUS))
			fn = tg3_interrupt_tagged;
		flags = IRQF_SHARED;
	}

	return request_irq(tnapi->irq_vec, fn, flags, name, tnapi);
}
