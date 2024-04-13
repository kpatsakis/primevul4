static int map_files_d_revalidate(struct dentry *dentry, unsigned int flags)
{
	unsigned long vm_start, vm_end;
	bool exact_vma_exists = false;
	struct mm_struct *mm = NULL;
	struct task_struct *task;
	const struct cred *cred;
	struct inode *inode;
	int status = 0;

	if (flags & LOOKUP_RCU)
		return -ECHILD;

	inode = d_inode(dentry);
	task = get_proc_task(inode);
	if (!task)
		goto out_notask;

	mm = mm_access(task, PTRACE_MODE_READ_FSCREDS);
	if (IS_ERR_OR_NULL(mm))
		goto out;

	if (!dname_to_vma_addr(dentry, &vm_start, &vm_end)) {
		down_read(&mm->mmap_sem);
		exact_vma_exists = !!find_exact_vma(mm, vm_start, vm_end);
		up_read(&mm->mmap_sem);
	}

	mmput(mm);

	if (exact_vma_exists) {
		if (task_dumpable(task)) {
			rcu_read_lock();
			cred = __task_cred(task);
			inode->i_uid = cred->euid;
			inode->i_gid = cred->egid;
			rcu_read_unlock();
		} else {
			inode->i_uid = GLOBAL_ROOT_UID;
			inode->i_gid = GLOBAL_ROOT_GID;
		}
		security_task_to_inode(task, inode);
		status = 1;
	}

out:
	put_task_struct(task);

out_notask:
	return status;
}
