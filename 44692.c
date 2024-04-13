static void fd_watchdog(void)
{
	debug_dcl(DP->flags, "calling disk change from watchdog\n");

	if (disk_change(current_drive)) {
		DPRINT("disk removed during i/o\n");
		cancel_activity();
		cont->done(0);
		reset_fdc();
	} else {
		cancel_delayed_work(&fd_timer);
		fd_timer_fn = fd_watchdog;
		queue_delayed_work(floppy_wq, &fd_timer, HZ / 10);
	}
}
