static int bm_fill_super(struct super_block * sb, void * data, int silent)
{
	static struct tree_descr bm_files[] = {
		[2] = {"status", &bm_status_operations, S_IWUSR|S_IRUGO},
		[3] = {"register", &bm_register_operations, S_IWUSR},
		/* last one */ {""}
	};
	int err = simple_fill_super(sb, BINFMTFS_MAGIC, bm_files);
	if (!err)
		sb->s_op = &s_ops;
	return err;
}
