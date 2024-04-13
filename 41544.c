static void tg3_timer_stop(struct tg3 *tp)
{
	del_timer_sync(&tp->timer);
}
