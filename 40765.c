static __init void intel_sandybridge_quirk(void)
{
	x86_pmu.check_microcode = intel_snb_check_microcode;
	intel_snb_check_microcode();
}
