static void option_active_branches(const char *branches)
{
	max_active_branches = ulong_arg("--active-branches", branches);
}
