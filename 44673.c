static void __init config_types(void)
{
	bool has_drive = false;
	int drive;

	/* read drive info out of physical CMOS */
	drive = 0;
	if (!UDP->cmos)
		UDP->cmos = FLOPPY0_TYPE;
	drive = 1;
	if (!UDP->cmos && FLOPPY1_TYPE)
		UDP->cmos = FLOPPY1_TYPE;

	/* FIXME: additional physical CMOS drive detection should go here */

	for (drive = 0; drive < N_DRIVE; drive++) {
		unsigned int type = UDP->cmos;
		struct floppy_drive_params *params;
		const char *name = NULL;
		static char temparea[32];

		if (type < ARRAY_SIZE(default_drive_params)) {
			params = &default_drive_params[type].params;
			if (type) {
				name = default_drive_params[type].name;
				allowed_drive_mask |= 1 << drive;
			} else
				allowed_drive_mask &= ~(1 << drive);
		} else {
			params = &default_drive_params[0].params;
			sprintf(temparea, "unknown type %d (usb?)", type);
			name = temparea;
		}
		if (name) {
			const char *prepend;
			if (!has_drive) {
				prepend = "";
				has_drive = true;
				pr_info("Floppy drive(s):");
			} else {
				prepend = ",";
			}

			pr_cont("%s fd%d is %s", prepend, drive, name);
		}
		*UDP = *params;
	}

	if (has_drive)
		pr_cont("\n");
}
