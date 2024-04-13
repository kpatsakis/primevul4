struct mm_struct *proc_mem_open(struct inode *inode, unsigned int mode)
{
	struct task_struct *task = get_proc_task(inode);
	struct mm_struct *mm = ERR_PTR(-ESRCH);

	if (task) {
		mm = mm_access(task, mode | PTRACE_MODE_FSCREDS);
		put_task_struct(task);

		if (!IS_ERR_OR_NULL(mm)) {
			/* ensure this mm_struct can't be freed */
			atomic_inc(&mm->mm_count);
			/* but do not pin its memory */
			mmput(mm);
		}
	}

	return mm;
}
