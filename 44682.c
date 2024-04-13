static void do_wakeup(void)
{
	reschedule_timeout(MAXTIMEOUT, "do wakeup");
	cont = NULL;
	command_status += 2;
	wake_up(&command_done);
}
