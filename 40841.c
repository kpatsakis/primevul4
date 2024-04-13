static int ptrace_signal(int signr, siginfo_t *info)
{
	ptrace_signal_deliver();
	/*
	 * We do not check sig_kernel_stop(signr) but set this marker
	 * unconditionally because we do not know whether debugger will
	 * change signr. This flag has no meaning unless we are going
	 * to stop after return from ptrace_stop(). In this case it will
	 * be checked in do_signal_stop(), we should only stop if it was
	 * not cleared by SIGCONT while we were sleeping. See also the
	 * comment in dequeue_signal().
	 */
	current->jobctl |= JOBCTL_STOP_DEQUEUED;
	ptrace_stop(signr, CLD_TRAPPED, 0, info);

	/* We're back.  Did the debugger cancel the sig?  */
	signr = current->exit_code;
	if (signr == 0)
		return signr;

	current->exit_code = 0;

	/*
	 * Update the siginfo structure if the signal has
	 * changed.  If the debugger wanted something
	 * specific in the siginfo structure then it should
	 * have updated *info via PTRACE_SETSIGINFO.
	 */
	if (signr != info->si_signo) {
		info->si_signo = signr;
		info->si_errno = 0;
		info->si_code = SI_USER;
		rcu_read_lock();
		info->si_pid = task_pid_vnr(current->parent);
		info->si_uid = from_kuid_munged(current_user_ns(),
						task_uid(current->parent));
		rcu_read_unlock();
	}

	/* If the (new) signal is now blocked, requeue it.  */
	if (sigismember(&current->blocked, signr)) {
		specific_send_sig_info(signr, info, current);
		signr = 0;
	}

	return signr;
}
