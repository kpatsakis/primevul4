static void parse_option(const char *option)
{
	if (seen_data_command)
		die("Got option command '%s' after data command", option);

	if (parse_one_option(option))
		return;

	die("This version of fast-import does not support option: %s", option);
}
