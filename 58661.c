void shrink_dcache_for_umount(struct super_block *sb)
{
	struct dentry *dentry;

	WARN(down_read_trylock(&sb->s_umount), "s_umount should've been locked");

	dentry = sb->s_root;
	sb->s_root = NULL;
	do_one_tree(dentry);

	while (!hlist_bl_empty(&sb->s_anon)) {
		dentry = dget(hlist_bl_entry(hlist_bl_first(&sb->s_anon), struct dentry, d_hash));
		do_one_tree(dentry);
	}
}
