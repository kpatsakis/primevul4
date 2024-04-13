static void tg3_timer_start(struct tg3 *tp)
{
	tp->asf_counter   = tp->asf_multiplier;
	tp->timer_counter = tp->timer_multiplier;

	tp->timer.expires = jiffies + tp->timer_offset;
	add_timer(&tp->timer);
}
