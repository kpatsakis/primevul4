int f2fs_inode_dirtied(struct inode *inode, bool sync)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	int ret = 0;

	spin_lock(&sbi->inode_lock[DIRTY_META]);
	if (is_inode_flag_set(inode, FI_DIRTY_INODE)) {
		ret = 1;
	} else {
		set_inode_flag(inode, FI_DIRTY_INODE);
		stat_inc_dirty_inode(sbi, DIRTY_META);
	}
	if (sync && list_empty(&F2FS_I(inode)->gdirty_list)) {
		list_add_tail(&F2FS_I(inode)->gdirty_list,
				&sbi->inode_list[DIRTY_META]);
		inc_page_count(sbi, F2FS_DIRTY_IMETA);
	}
	spin_unlock(&sbi->inode_lock[DIRTY_META]);
	return ret;
}
