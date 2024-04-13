static const char *perf_etc_perfconfig(void)
{
	static const char *system_wide;
	if (!system_wide)
		system_wide = system_path(ETC_PERFCONFIG);
	return system_wide;
}
