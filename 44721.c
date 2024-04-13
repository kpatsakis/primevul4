static void floppy_start(void)
{
	reschedule_timeout(current_reqD, "floppy start");

	scandrives();
	debug_dcl(DP->flags, "setting NEWCHANGE in floppy_start\n");
	set_bit(FD_DISK_NEWCHANGE_BIT, &DRS->flags);
	floppy_ready();
}
