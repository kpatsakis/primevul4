static int syslog_action_restricted(int type)
{
	if (dmesg_restrict)
		return 1;
	/* Unless restricted, we allow "read all" and "get buffer size" for everybody */
	return type != SYSLOG_ACTION_READ_ALL && type != SYSLOG_ACTION_SIZE_BUFFER;
}
