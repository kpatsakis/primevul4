static long do_splice_from(struct pipe_inode_info *pipe, struct file *out,
			   loff_t *ppos, size_t len, unsigned int flags)
{
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *,
				loff_t *, size_t, unsigned int);

	if (out->f_op->splice_write)
		splice_write = out->f_op->splice_write;
	else
		splice_write = default_file_splice_write;

	return splice_write(pipe, out, ppos, len, flags);
}
