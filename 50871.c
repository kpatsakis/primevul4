static int proc_task_readdir(struct file *file, struct dir_context *ctx)
{
	struct inode *inode = file_inode(file);
	struct task_struct *task;
	struct pid_namespace *ns;
	int tid;

	if (proc_inode_is_dead(inode))
		return -ENOENT;

	if (!dir_emit_dots(file, ctx))
		return 0;

	/* f_version caches the tgid value that the last readdir call couldn't
	 * return. lseek aka telldir automagically resets f_version to 0.
	 */
	ns = inode->i_sb->s_fs_info;
	tid = (int)file->f_version;
	file->f_version = 0;
	for (task = first_tid(proc_pid(inode), tid, ctx->pos - 2, ns);
	     task;
	     task = next_tid(task), ctx->pos++) {
		char name[PROC_NUMBUF];
		int len;
		tid = task_pid_nr_ns(task, ns);
		len = snprintf(name, sizeof(name), "%d", tid);
		if (!proc_fill_cache(file, ctx, name, len,
				proc_task_instantiate, task, NULL)) {
			/* returning this tgid failed, save it as the first
			 * pid for the next readir call */
			file->f_version = (u64)tid;
			put_task_struct(task);
			break;
		}
	}

	return 0;
}
