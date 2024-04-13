static void is_alive(const char *func, const char *message)
{
	/* this routine checks whether the floppy driver is "alive" */
	if (test_bit(0, &fdc_busy) && command_status < 2 &&
	    !delayed_work_pending(&fd_timeout)) {
		DPRINT("%s: timeout handler died.  %s\n", func, message);
	}
}
