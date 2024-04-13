static inline void tg3_reset_task_schedule(struct tg3 *tp)
{
	if (!test_and_set_bit(TG3_FLAG_RESET_TASK_PENDING, tp->tg3_flags))
		schedule_work(&tp->reset_task);
}
