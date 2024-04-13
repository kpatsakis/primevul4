static void tsc_bad(void *info)
{
	__get_cpu_var(cpu_tsc_khz) = 0;
}
