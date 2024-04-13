static void fuse_writepages_send(struct fuse_fill_wb_data *data)
{
	struct fuse_req *req = data->req;
	struct inode *inode = data->inode;
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_inode *fi = get_fuse_inode(inode);
	int num_pages = req->num_pages;
	int i;

	req->ff = fuse_file_get(data->ff);
	spin_lock(&fc->lock);
	list_add_tail(&req->list, &fi->queued_writes);
	fuse_flush_writepages(inode);
	spin_unlock(&fc->lock);

	for (i = 0; i < num_pages; i++)
		end_page_writeback(data->orig_pages[i]);
}
