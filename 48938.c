static int ubifs_writepage(struct page *page, struct writeback_control *wbc)
{
	struct inode *inode = page->mapping->host;
	struct ubifs_inode *ui = ubifs_inode(inode);
	loff_t i_size =  i_size_read(inode), synced_i_size;
	pgoff_t end_index = i_size >> PAGE_CACHE_SHIFT;
	int err, len = i_size & (PAGE_CACHE_SIZE - 1);
	void *kaddr;

	dbg_gen("ino %lu, pg %lu, pg flags %#lx",
		inode->i_ino, page->index, page->flags);
	ubifs_assert(PagePrivate(page));

	/* Is the page fully outside @i_size? (truncate in progress) */
	if (page->index > end_index || (page->index == end_index && !len)) {
		err = 0;
		goto out_unlock;
	}

	spin_lock(&ui->ui_lock);
	synced_i_size = ui->synced_i_size;
	spin_unlock(&ui->ui_lock);

	/* Is the page fully inside @i_size? */
	if (page->index < end_index) {
		if (page->index >= synced_i_size >> PAGE_CACHE_SHIFT) {
			err = inode->i_sb->s_op->write_inode(inode, NULL);
			if (err)
				goto out_unlock;
			/*
			 * The inode has been written, but the write-buffer has
			 * not been synchronized, so in case of an unclean
			 * reboot we may end up with some pages beyond inode
			 * size, but they would be in the journal (because
			 * commit flushes write buffers) and recovery would deal
			 * with this.
			 */
		}
		return do_writepage(page, PAGE_CACHE_SIZE);
	}

	/*
	 * The page straddles @i_size. It must be zeroed out on each and every
	 * writepage invocation because it may be mmapped. "A file is mapped
	 * in multiples of the page size. For a file that is not a multiple of
	 * the page size, the remaining memory is zeroed when mapped, and
	 * writes to that region are not written out to the file."
	 */
	kaddr = kmap_atomic(page);
	memset(kaddr + len, 0, PAGE_CACHE_SIZE - len);
	flush_dcache_page(page);
	kunmap_atomic(kaddr);

	if (i_size > synced_i_size) {
		err = inode->i_sb->s_op->write_inode(inode, NULL);
		if (err)
			goto out_unlock;
	}

	return do_writepage(page, len);

out_unlock:
	unlock_page(page);
	return err;
}
