static int disk_change(int drive)
{
	int fdc = FDC(drive);

	if (time_before(jiffies, UDRS->select_date + UDP->select_delay))
		DPRINT("WARNING disk change called early\n");
	if (!(FDCS->dor & (0x10 << UNIT(drive))) ||
	    (FDCS->dor & 3) != UNIT(drive) || fdc != FDC(drive)) {
		DPRINT("probing disk change on unselected drive\n");
		DPRINT("drive=%d fdc=%d dor=%x\n", drive, FDC(drive),
		       (unsigned int)FDCS->dor);
	}

	debug_dcl(UDP->flags,
		  "checking disk change line for drive %d\n", drive);
	debug_dcl(UDP->flags, "jiffies=%lu\n", jiffies);
	debug_dcl(UDP->flags, "disk change line=%x\n", fd_inb(FD_DIR) & 0x80);
	debug_dcl(UDP->flags, "flags=%lx\n", UDRS->flags);

	if (UDP->flags & FD_BROKEN_DCL)
		return test_bit(FD_DISK_CHANGED_BIT, &UDRS->flags);
	if ((fd_inb(FD_DIR) ^ UDP->flags) & 0x80) {
		set_bit(FD_VERIFY_BIT, &UDRS->flags);
					/* verify write protection */

		if (UDRS->maxblock)	/* mark it changed */
			set_bit(FD_DISK_CHANGED_BIT, &UDRS->flags);

		/* invalidate its geometry */
		if (UDRS->keep_data >= 0) {
			if ((UDP->flags & FTD_MSG) &&
			    current_type[drive] != NULL)
				DPRINT("Disk type is undefined after disk change\n");
			current_type[drive] = NULL;
			floppy_sizes[TOMINOR(drive)] = MAX_DISK_SIZE << 1;
		}

		return 1;
	} else {
		UDRS->last_checked = jiffies;
		clear_bit(FD_DISK_NEWCHANGE_BIT, &UDRS->flags);
	}
	return 0;
}
