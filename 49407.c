static bool __init avx2_usable(void)
{
	if (avx_usable() && cpu_has_avx2 && boot_cpu_has(X86_FEATURE_BMI1) &&
	    boot_cpu_has(X86_FEATURE_BMI2))
		return true;

	return false;
}
