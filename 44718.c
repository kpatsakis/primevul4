static void __init floppy_set_flags(int *ints, int param, int param2)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(default_drive_params); i++) {
		if (param)
			default_drive_params[i].params.flags |= param2;
		else
			default_drive_params[i].params.flags &= ~param2;
	}
	DPRINT("%s flag 0x%x\n", param2 ? "Setting" : "Clearing", param);
}
