static struct dentry *proc_map_files_lookup(struct inode *dir,
		struct dentry *dentry, unsigned int flags)
{
	unsigned long vm_start, vm_end;
	struct vm_area_struct *vma;
	struct task_struct *task;
	int result;
	struct mm_struct *mm;

	result = -ENOENT;
	task = get_proc_task(dir);
	if (!task)
		goto out;

	result = -EACCES;
	if (!ptrace_may_access(task, PTRACE_MODE_READ_FSCREDS))
		goto out_put_task;

	result = -ENOENT;
	if (dname_to_vma_addr(dentry, &vm_start, &vm_end))
		goto out_put_task;

	mm = get_task_mm(task);
	if (!mm)
		goto out_put_task;

	down_read(&mm->mmap_sem);
	vma = find_exact_vma(mm, vm_start, vm_end);
	if (!vma)
		goto out_no_vma;

	if (vma->vm_file)
		result = proc_map_files_instantiate(dir, dentry, task,
				(void *)(unsigned long)vma->vm_file->f_mode);

out_no_vma:
	up_read(&mm->mmap_sem);
	mmput(mm);
out_put_task:
	put_task_struct(task);
out:
	return ERR_PTR(result);
}
