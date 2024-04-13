static void __reschedule_timeout(int drive, const char *message)
{
	unsigned long delay;

	if (drive == current_reqD)
		drive = current_drive;

	if (drive < 0 || drive >= N_DRIVE) {
		delay = 20UL * HZ;
		drive = 0;
	} else
		delay = UDP->timeout;

	mod_delayed_work(floppy_wq, &fd_timeout, delay);
	if (UDP->flags & FD_DEBUG)
		DPRINT("reschedule timeout %s\n", message);
	timeout_message = message;
}
