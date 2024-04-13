static struct nfs_page * nfs_setup_write_request(struct nfs_open_context* ctx,
		struct page *page, unsigned int offset, unsigned int bytes)
{
	struct inode *inode = page_file_mapping(page)->host;
	struct nfs_page	*req;

	req = nfs_try_to_update_request(inode, page, offset, bytes);
	if (req != NULL)
		goto out;
	req = nfs_create_request(ctx, inode, page, offset, bytes);
	if (IS_ERR(req))
		goto out;
	nfs_inode_add_request(inode, req);
out:
	return req;
}
