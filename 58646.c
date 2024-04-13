static void get_fs_root_rcu(struct fs_struct *fs, struct path *root)
{
	unsigned seq;

	do {
		seq = read_seqcount_begin(&fs->seq);
		*root = fs->root;
	} while (read_seqcount_retry(&fs->seq, seq));
}
