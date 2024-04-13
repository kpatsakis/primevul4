static int isofs_readpage(struct file *file, struct page *page)
{
	return mpage_readpage(page, isofs_get_block);
}
