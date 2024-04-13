static void __init daring(int *ints, int param, int param2)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(default_drive_params); i++) {
		if (param) {
			default_drive_params[i].params.select_delay = 0;
			default_drive_params[i].params.flags |=
			    FD_SILENT_DCL_CLEAR;
		} else {
			default_drive_params[i].params.select_delay =
			    2 * HZ / 100;
			default_drive_params[i].params.flags &=
			    ~FD_SILENT_DCL_CLEAR;
		}
	}
	DPRINT("Assuming %s floppy hardware\n", param ? "standard" : "broken");
}
