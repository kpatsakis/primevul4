SYSCALL_DEFINE3(get_robust_list, int, pid,
		struct robust_list_head __user * __user *, head_ptr,
		size_t __user *, len_ptr)
{
	struct robust_list_head __user *head;
	unsigned long ret;
	const struct cred *cred = current_cred(), *pcred;

	if (!futex_cmpxchg_enabled)
		return -ENOSYS;

	if (!pid)
		head = current->robust_list;
	else {
		struct task_struct *p;

		ret = -ESRCH;
		rcu_read_lock();
		p = find_task_by_vpid(pid);
		if (!p)
			goto err_unlock;
		ret = -EPERM;
		pcred = __task_cred(p);
		if (cred->euid != pcred->euid &&
		    cred->euid != pcred->uid &&
		    !capable(CAP_SYS_PTRACE))
			goto err_unlock;
		head = p->robust_list;
		rcu_read_unlock();
	}

	if (put_user(sizeof(*head), len_ptr))
		return -EFAULT;
	return put_user(head, head_ptr);

err_unlock:
	rcu_read_unlock();

	return ret;
}
