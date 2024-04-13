static void __init set_cmos(int *ints, int dummy, int dummy2)
{
	int current_drive = 0;

	if (ints[0] != 2) {
		DPRINT("wrong number of parameters for CMOS\n");
		return;
	}
	current_drive = ints[1];
	if (current_drive < 0 || current_drive >= 8) {
		DPRINT("bad drive for set_cmos\n");
		return;
	}
#if N_FDC > 1
	if (current_drive >= 4 && !FDC2)
		FDC2 = 0x370;
#endif
	DP->cmos = ints[2];
	DPRINT("setting CMOS code to %d\n", ints[2]);
}
