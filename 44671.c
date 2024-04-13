static void cancel_activity(void)
{
	do_floppy = NULL;
	cancel_delayed_work_sync(&fd_timer);
	cancel_work_sync(&floppy_work);
}
