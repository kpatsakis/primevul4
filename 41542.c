static void tg3_timer_init(struct tg3 *tp)
{
	if (tg3_flag(tp, TAGGED_STATUS) &&
	    tg3_asic_rev(tp) != ASIC_REV_5717 &&
	    !tg3_flag(tp, 57765_CLASS))
		tp->timer_offset = HZ;
	else
		tp->timer_offset = HZ / 10;

	BUG_ON(tp->timer_offset > HZ);

	tp->timer_multiplier = (HZ / tp->timer_offset);
	tp->asf_multiplier = (HZ / tp->timer_offset) *
			     TG3_FW_UPDATE_FREQ_SEC;

	init_timer(&tp->timer);
	tp->timer.data = (unsigned long) tp;
	tp->timer.function = tg3_timer;
}
