static int exec_binprm(struct linux_binprm *bprm)
{
	pid_t old_pid, old_vpid;
	int ret;

	/* Need to fetch pid before load_binary changes it */
	old_pid = current->pid;
	rcu_read_lock();
	old_vpid = task_pid_nr_ns(current, task_active_pid_ns(current->parent));
	rcu_read_unlock();

	ret = search_binary_handler(bprm);
	if (ret >= 0) {
		trace_sched_process_exec(current, old_pid, bprm);
		ptrace_event(PTRACE_EVENT_EXEC, old_vpid);
		current->did_exec = 1;
		proc_exec_connector(current);

		if (bprm->file) {
			allow_write_access(bprm->file);
			fput(bprm->file);
			bprm->file = NULL; /* to catch use-after-free */
		}
	}

	return ret;
}
