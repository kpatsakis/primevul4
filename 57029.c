static void default_options(struct f2fs_sb_info *sbi)
{
	/* init some FS parameters */
	sbi->active_logs = NR_CURSEG_TYPE;

	set_opt(sbi, BG_GC);
	set_opt(sbi, INLINE_XATTR);
	set_opt(sbi, INLINE_DATA);
	set_opt(sbi, INLINE_DENTRY);
	set_opt(sbi, EXTENT_CACHE);
	set_opt(sbi, NOHEAP);
	sbi->sb->s_flags |= MS_LAZYTIME;
	set_opt(sbi, FLUSH_MERGE);
	if (f2fs_sb_mounted_blkzoned(sbi->sb)) {
		set_opt_mode(sbi, F2FS_MOUNT_LFS);
		set_opt(sbi, DISCARD);
	} else {
		set_opt_mode(sbi, F2FS_MOUNT_ADAPTIVE);
	}

#ifdef CONFIG_F2FS_FS_XATTR
	set_opt(sbi, XATTR_USER);
#endif
#ifdef CONFIG_F2FS_FS_POSIX_ACL
	set_opt(sbi, POSIX_ACL);
#endif

#ifdef CONFIG_F2FS_FAULT_INJECTION
	f2fs_build_fault_attr(sbi, 0);
#endif
}
