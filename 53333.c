static void session_clear_tty(struct pid *session)
{
	struct task_struct *p;
	do_each_pid_task(session, PIDTYPE_SID, p) {
		proc_clear_tty(p);
	} while_each_pid_task(session, PIDTYPE_SID, p);
}
