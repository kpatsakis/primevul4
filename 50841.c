bool proc_fill_cache(struct file *file, struct dir_context *ctx,
	const char *name, int len,
	instantiate_t instantiate, struct task_struct *task, const void *ptr)
{
	struct dentry *child, *dir = file->f_path.dentry;
	struct qstr qname = QSTR_INIT(name, len);
	struct inode *inode;
	unsigned type;
	ino_t ino;

	child = d_hash_and_lookup(dir, &qname);
	if (!child) {
		child = d_alloc(dir, &qname);
		if (!child)
			goto end_instantiate;
		if (instantiate(d_inode(dir), child, task, ptr) < 0) {
			dput(child);
			goto end_instantiate;
		}
	}
	inode = d_inode(child);
	ino = inode->i_ino;
	type = inode->i_mode >> 12;
	dput(child);
	return dir_emit(ctx, name, len, ino, type);

end_instantiate:
	return dir_emit(ctx, name, len, 1, DT_UNKNOWN);
}
