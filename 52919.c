static void option_depth(const char *depth)
{
	max_depth = ulong_arg("--depth", depth);
	if (max_depth > MAX_DEPTH)
		die("--depth cannot exceed %u", MAX_DEPTH);
}
