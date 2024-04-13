static int f2fs_show_options(struct seq_file *seq, struct dentry *root)
{
	struct f2fs_sb_info *sbi = F2FS_SB(root->d_sb);

	if (!f2fs_readonly(sbi->sb) && test_opt(sbi, BG_GC)) {
		if (test_opt(sbi, FORCE_FG_GC))
			seq_printf(seq, ",background_gc=%s", "sync");
		else
			seq_printf(seq, ",background_gc=%s", "on");
	} else {
		seq_printf(seq, ",background_gc=%s", "off");
	}
	if (test_opt(sbi, DISABLE_ROLL_FORWARD))
		seq_puts(seq, ",disable_roll_forward");
	if (test_opt(sbi, DISCARD))
		seq_puts(seq, ",discard");
	if (test_opt(sbi, NOHEAP))
		seq_puts(seq, ",no_heap");
	else
		seq_puts(seq, ",heap");
#ifdef CONFIG_F2FS_FS_XATTR
	if (test_opt(sbi, XATTR_USER))
		seq_puts(seq, ",user_xattr");
	else
		seq_puts(seq, ",nouser_xattr");
	if (test_opt(sbi, INLINE_XATTR))
		seq_puts(seq, ",inline_xattr");
	else
		seq_puts(seq, ",noinline_xattr");
#endif
#ifdef CONFIG_F2FS_FS_POSIX_ACL
	if (test_opt(sbi, POSIX_ACL))
		seq_puts(seq, ",acl");
	else
		seq_puts(seq, ",noacl");
#endif
	if (test_opt(sbi, DISABLE_EXT_IDENTIFY))
		seq_puts(seq, ",disable_ext_identify");
	if (test_opt(sbi, INLINE_DATA))
		seq_puts(seq, ",inline_data");
	else
		seq_puts(seq, ",noinline_data");
	if (test_opt(sbi, INLINE_DENTRY))
		seq_puts(seq, ",inline_dentry");
	else
		seq_puts(seq, ",noinline_dentry");
	if (!f2fs_readonly(sbi->sb) && test_opt(sbi, FLUSH_MERGE))
		seq_puts(seq, ",flush_merge");
	if (test_opt(sbi, NOBARRIER))
		seq_puts(seq, ",nobarrier");
	if (test_opt(sbi, FASTBOOT))
		seq_puts(seq, ",fastboot");
	if (test_opt(sbi, EXTENT_CACHE))
		seq_puts(seq, ",extent_cache");
	else
		seq_puts(seq, ",noextent_cache");
	if (test_opt(sbi, DATA_FLUSH))
		seq_puts(seq, ",data_flush");

	seq_puts(seq, ",mode=");
	if (test_opt(sbi, ADAPTIVE))
		seq_puts(seq, "adaptive");
	else if (test_opt(sbi, LFS))
		seq_puts(seq, "lfs");
	seq_printf(seq, ",active_logs=%u", sbi->active_logs);
	if (F2FS_IO_SIZE_BITS(sbi))
		seq_printf(seq, ",io_size=%uKB", F2FS_IO_SIZE_KB(sbi));
#ifdef CONFIG_F2FS_FAULT_INJECTION
	if (test_opt(sbi, FAULT_INJECTION))
		seq_puts(seq, ",fault_injection");
#endif

	return 0;
}
