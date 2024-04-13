static int f2fs_vm_page_mkwrite(struct vm_area_struct *vma,
						struct vm_fault *vmf)
{
	struct page *page = vmf->page;
	struct inode *inode = file_inode(vma->vm_file);
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
	struct dnode_of_data dn;
	int err;

	f2fs_balance_fs(sbi);

	sb_start_pagefault(inode->i_sb);

	/* block allocation */
	f2fs_lock_op(sbi);
	set_new_dnode(&dn, inode, NULL, NULL, 0);
	err = f2fs_reserve_block(&dn, page->index);
	f2fs_unlock_op(sbi);
	if (err)
		goto out;

	file_update_time(vma->vm_file);
	lock_page(page);
	if (unlikely(page->mapping != inode->i_mapping ||
			page_offset(page) > i_size_read(inode) ||
			!PageUptodate(page))) {
		unlock_page(page);
		err = -EFAULT;
		goto out;
	}

	/*
	 * check to see if the page is mapped already (no holes)
	 */
	if (PageMappedToDisk(page))
		goto mapped;

	/* page is wholly or partially inside EOF */
	if (((page->index + 1) << PAGE_CACHE_SHIFT) > i_size_read(inode)) {
		unsigned offset;
		offset = i_size_read(inode) & ~PAGE_CACHE_MASK;
		zero_user_segment(page, offset, PAGE_CACHE_SIZE);
	}
	set_page_dirty(page);
	SetPageUptodate(page);

	trace_f2fs_vm_page_mkwrite(page, DATA);
mapped:
	/* fill the page */
	f2fs_wait_on_page_writeback(page, DATA);
out:
	sb_end_pagefault(inode->i_sb);
	return block_page_mkwrite_return(err);
}
