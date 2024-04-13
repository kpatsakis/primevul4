static void tty_warn_deprecated_flags(struct serial_struct __user *ss)
{
	static DEFINE_RATELIMIT_STATE(depr_flags,
			DEFAULT_RATELIMIT_INTERVAL,
			DEFAULT_RATELIMIT_BURST);
	char comm[TASK_COMM_LEN];
	int flags;

	if (get_user(flags, &ss->flags))
		return;

	flags &= ASYNC_DEPRECATED;

	if (flags && __ratelimit(&depr_flags))
		pr_warning("%s: '%s' is using deprecated serial flags (with no effect): %.8x\n",
				__func__, get_task_comm(comm, current), flags);
}
