static void check_buildid_dir_config(void)
{
	struct buildid_dir_config c;
	c.dir = buildid_dir;
	perf_config(buildid_dir_command_config, &c);
}
