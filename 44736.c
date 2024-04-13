static int need_more_output(void)
{
	int status = wait_til_ready();

	if (status < 0)
		return -1;

	if (is_ready_state(status))
		return MORE_OUTPUT;

	return result();
}
