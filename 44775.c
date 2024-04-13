static void unlock_fdc(void)
{
	if (!test_bit(0, &fdc_busy))
		DPRINT("FDC access conflict!\n");

	raw_cmd = NULL;
	command_status = FD_COMMAND_NONE;
	cancel_delayed_work(&fd_timeout);
	do_floppy = NULL;
	cont = NULL;
	clear_bit(0, &fdc_busy);
	wake_up(&fdc_wait);
}
