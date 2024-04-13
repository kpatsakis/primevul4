static void fill_zero(struct inode *inode, pgoff_t index,
					loff_t start, loff_t len)
{
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
	struct page *page;

	if (!len)
		return;

	f2fs_balance_fs(sbi);

	f2fs_lock_op(sbi);
	page = get_new_data_page(inode, NULL, index, false);
	f2fs_unlock_op(sbi);

	if (!IS_ERR(page)) {
		f2fs_wait_on_page_writeback(page, DATA);
		zero_user(page, start, len);
		set_page_dirty(page);
		f2fs_put_page(page, 1);
	}
}
