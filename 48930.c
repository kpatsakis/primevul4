static int ubifs_readpage(struct file *file, struct page *page)
{
	if (ubifs_bulk_read(page))
		return 0;
	do_readpage(page);
	unlock_page(page);
	return 0;
}
