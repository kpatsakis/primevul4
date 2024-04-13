static int check_syslog_permissions(int type, bool from_file)
{
	/*
	 * If this is from /proc/kmsg and we've already opened it, then we've
	 * already done the capabilities checks at open time.
	 */
	if (from_file && type != SYSLOG_ACTION_OPEN)
		return 0;

	if (syslog_action_restricted(type)) {
		if (capable(CAP_SYSLOG))
			return 0;
		/* For historical reasons, accept CAP_SYS_ADMIN too, with a warning */
		if (capable(CAP_SYS_ADMIN)) {
			printk_once(KERN_WARNING "%s (%d): "
				 "Attempt to access syslog with CAP_SYS_ADMIN "
				 "but no CAP_SYSLOG (deprecated).\n",
				 current->comm, task_pid_nr(current));
			return 0;
		}
		return -EPERM;
	}
	return 0;
}
