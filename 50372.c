void proc_sys_poll_notify(struct ctl_table_poll *poll)
{
	if (!poll)
		return;

	atomic_inc(&poll->event);
	wake_up_interruptible(&poll->wait);
}
