static ssize_t proc_coredump_filter_write(struct file *file,
					  const char __user *buf,
					  size_t count,
					  loff_t *ppos)
{
	struct task_struct *task;
	struct mm_struct *mm;
	unsigned int val;
	int ret;
	int i;
	unsigned long mask;

	ret = kstrtouint_from_user(buf, count, 0, &val);
	if (ret < 0)
		return ret;

	ret = -ESRCH;
	task = get_proc_task(file_inode(file));
	if (!task)
		goto out_no_task;

	mm = get_task_mm(task);
	if (!mm)
		goto out_no_mm;
	ret = 0;

	for (i = 0, mask = 1; i < MMF_DUMP_FILTER_BITS; i++, mask <<= 1) {
		if (val & mask)
			set_bit(i + MMF_DUMP_FILTER_SHIFT, &mm->flags);
		else
			clear_bit(i + MMF_DUMP_FILTER_SHIFT, &mm->flags);
	}

	mmput(mm);
 out_no_mm:
	put_task_struct(task);
 out_no_task:
	if (ret < 0)
		return ret;
	return count;
}
