static __always_inline unsigned set_root_rcu(struct nameidata *nd)
{
	struct fs_struct *fs = current->fs;
	unsigned seq, res;

	do {
		seq = read_seqcount_begin(&fs->seq);
		nd->root = fs->root;
		res = __read_seqcount_begin(&nd->root.dentry->d_seq);
	} while (read_seqcount_retry(&fs->seq, seq));
	return res;
}
