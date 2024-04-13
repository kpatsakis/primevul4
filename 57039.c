static void f2fs_dirty_inode(struct inode *inode, int flags)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);

	if (inode->i_ino == F2FS_NODE_INO(sbi) ||
			inode->i_ino == F2FS_META_INO(sbi))
		return;

	if (flags == I_DIRTY_TIME)
		return;

	if (is_inode_flag_set(inode, FI_AUTO_RECOVER))
		clear_inode_flag(inode, FI_AUTO_RECOVER);

	f2fs_inode_dirtied(inode, false);
}
