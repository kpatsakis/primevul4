void f2fs_inode_synced(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);

	spin_lock(&sbi->inode_lock[DIRTY_META]);
	if (!is_inode_flag_set(inode, FI_DIRTY_INODE)) {
		spin_unlock(&sbi->inode_lock[DIRTY_META]);
		return;
	}
	if (!list_empty(&F2FS_I(inode)->gdirty_list)) {
		list_del_init(&F2FS_I(inode)->gdirty_list);
		dec_page_count(sbi, F2FS_DIRTY_IMETA);
	}
	clear_inode_flag(inode, FI_DIRTY_INODE);
	clear_inode_flag(inode, FI_AUTO_RECOVER);
	stat_dec_dirty_inode(F2FS_I_SB(inode), DIRTY_META);
	spin_unlock(&sbi->inode_lock[DIRTY_META]);
}
