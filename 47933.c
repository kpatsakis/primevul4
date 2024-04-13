static void process_lock_setup_atfork(void)
{
	pthread_atfork(process_lock, process_unlock, process_unlock);
}
