static void fuse_writepage_free(struct fuse_conn *fc, struct fuse_req *req)
{
	int i;

	for (i = 0; i < req->num_pages; i++)
		__free_page(req->pages[i]);

	if (req->ff)
		fuse_file_put(req->ff, false);
}
