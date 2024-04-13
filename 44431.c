void tm_reclaim_current(uint8_t cause)
{
	tm_enable();
	tm_reclaim_thread(&current->thread, current_thread_info(), cause);
}
