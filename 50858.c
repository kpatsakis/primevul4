int proc_pid_readdir(struct file *file, struct dir_context *ctx)
{
	struct tgid_iter iter;
	struct pid_namespace *ns = file_inode(file)->i_sb->s_fs_info;
	loff_t pos = ctx->pos;

	if (pos >= PID_MAX_LIMIT + TGID_OFFSET)
		return 0;

	if (pos == TGID_OFFSET - 2) {
		struct inode *inode = d_inode(ns->proc_self);
		if (!dir_emit(ctx, "self", 4, inode->i_ino, DT_LNK))
			return 0;
		ctx->pos = pos = pos + 1;
	}
	if (pos == TGID_OFFSET - 1) {
		struct inode *inode = d_inode(ns->proc_thread_self);
		if (!dir_emit(ctx, "thread-self", 11, inode->i_ino, DT_LNK))
			return 0;
		ctx->pos = pos = pos + 1;
	}
	iter.tgid = pos - TGID_OFFSET;
	iter.task = NULL;
	for (iter = next_tgid(ns, iter);
	     iter.task;
	     iter.tgid += 1, iter = next_tgid(ns, iter)) {
		char name[PROC_NUMBUF];
		int len;
		if (!has_pid_permissions(ns, iter.task, 2))
			continue;

		len = snprintf(name, sizeof(name), "%d", iter.tgid);
		ctx->pos = iter.tgid + TGID_OFFSET;
		if (!proc_fill_cache(file, ctx, name, len,
				     proc_pid_instantiate, iter.task, NULL)) {
			put_task_struct(iter.task);
			return 0;
		}
	}
	ctx->pos = PID_MAX_LIMIT + TGID_OFFSET;
	return 0;
}
