static int perf_config_system(void)
{
	return !perf_env_bool("PERF_CONFIG_NOSYSTEM", 0);
}
