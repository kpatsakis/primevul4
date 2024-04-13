static inline void tg3_reset_task_cancel(struct tg3 *tp)
{
	cancel_work_sync(&tp->reset_task);
	tg3_flag_clear(tp, RESET_TASK_PENDING);
	tg3_flag_clear(tp, TX_RECOVERY_PENDING);
}
