static int ptrace_peek_siginfo(struct task_struct *child,
				unsigned long addr,
				unsigned long data)
{
	struct ptrace_peeksiginfo_args arg;
	struct sigpending *pending;
	struct sigqueue *q;
	int ret, i;

	ret = copy_from_user(&arg, (void __user *) addr,
				sizeof(struct ptrace_peeksiginfo_args));
	if (ret)
		return -EFAULT;

	if (arg.flags & ~PTRACE_PEEKSIGINFO_SHARED)
		return -EINVAL; /* unknown flags */

	if (arg.nr < 0)
		return -EINVAL;

	if (arg.flags & PTRACE_PEEKSIGINFO_SHARED)
		pending = &child->signal->shared_pending;
	else
		pending = &child->pending;

	for (i = 0; i < arg.nr; ) {
		siginfo_t info;
		s32 off = arg.off + i;

		spin_lock_irq(&child->sighand->siglock);
		list_for_each_entry(q, &pending->list, list) {
			if (!off--) {
				copy_siginfo(&info, &q->info);
				break;
			}
		}
		spin_unlock_irq(&child->sighand->siglock);

		if (off >= 0) /* beyond the end of the list */
			break;

#ifdef CONFIG_COMPAT
		if (unlikely(is_compat_task())) {
			compat_siginfo_t __user *uinfo = compat_ptr(data);

			if (copy_siginfo_to_user32(uinfo, &info) ||
			    __put_user(info.si_code, &uinfo->si_code)) {
				ret = -EFAULT;
				break;
			}

		} else
#endif
		{
			siginfo_t __user *uinfo = (siginfo_t __user *) data;

			if (copy_siginfo_to_user(uinfo, &info) ||
			    __put_user(info.si_code, &uinfo->si_code)) {
				ret = -EFAULT;
				break;
			}
		}

		data += sizeof(siginfo_t);
		i++;

		if (signal_pending(current))
			break;

		cond_resched();
	}

	if (i > 0)
		return i;

	return ret;
}
