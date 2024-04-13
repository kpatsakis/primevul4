int pid_getattr(struct vfsmount *mnt, struct dentry *dentry, struct kstat *stat)
{
	struct inode *inode = d_inode(dentry);
	struct task_struct *task;
	const struct cred *cred;
	struct pid_namespace *pid = dentry->d_sb->s_fs_info;

	generic_fillattr(inode, stat);

	rcu_read_lock();
	stat->uid = GLOBAL_ROOT_UID;
	stat->gid = GLOBAL_ROOT_GID;
	task = pid_task(proc_pid(inode), PIDTYPE_PID);
	if (task) {
		if (!has_pid_permissions(pid, task, 2)) {
			rcu_read_unlock();
			/*
			 * This doesn't prevent learning whether PID exists,
			 * it only makes getattr() consistent with readdir().
			 */
			return -ENOENT;
		}
		if ((inode->i_mode == (S_IFDIR|S_IRUGO|S_IXUGO)) ||
		    task_dumpable(task)) {
			cred = __task_cred(task);
			stat->uid = cred->euid;
			stat->gid = cred->egid;
		}
	}
	rcu_read_unlock();
	return 0;
}
