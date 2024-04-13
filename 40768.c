static int intel_snb_pebs_broken(int cpu)
{
	u32 rev = UINT_MAX; /* default to broken for unknown models */

	switch (cpu_data(cpu).x86_model) {
	case 42: /* SNB */
		rev = 0x28;
		break;

	case 45: /* SNB-EP */
		switch (cpu_data(cpu).x86_mask) {
		case 6: rev = 0x618; break;
		case 7: rev = 0x70c; break;
		}
	}

	return (cpu_data(cpu).microcode < rev);
}
