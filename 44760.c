static void seek_interrupt(void)
{
	debugt(__func__, "");
	if (inr != 2 || (ST0 & 0xF8) != 0x20) {
		DPRINT("seek failed\n");
		DRS->track = NEED_2_RECAL;
		cont->error();
		cont->redo();
		return;
	}
	if (DRS->track >= 0 && DRS->track != ST1 && !blind_seek) {
		debug_dcl(DP->flags,
			  "clearing NEWCHANGE flag because of effective seek\n");
		debug_dcl(DP->flags, "jiffies=%lu\n", jiffies);
		clear_bit(FD_DISK_NEWCHANGE_BIT, &DRS->flags);
					/* effective seek */
		DRS->select_date = jiffies;
	}
	DRS->track = ST1;
	floppy_ready();
}
