static int cuse_send_init(struct cuse_conn *cc)
{
	int rc;
	struct fuse_req *req;
	struct page *page;
	struct fuse_conn *fc = &cc->fc;
	struct cuse_init_in *arg;
	void *outarg;

	BUILD_BUG_ON(CUSE_INIT_INFO_MAX > PAGE_SIZE);

	req = fuse_get_req_for_background(fc, 1);
	if (IS_ERR(req)) {
		rc = PTR_ERR(req);
		goto err;
	}

	rc = -ENOMEM;
	page = alloc_page(GFP_KERNEL | __GFP_ZERO);
	if (!page)
		goto err_put_req;

	outarg = kzalloc(sizeof(struct cuse_init_out), GFP_KERNEL);
	if (!outarg)
		goto err_free_page;

	arg = &req->misc.cuse_init_in;
	arg->major = FUSE_KERNEL_VERSION;
	arg->minor = FUSE_KERNEL_MINOR_VERSION;
	arg->flags |= CUSE_UNRESTRICTED_IOCTL;
	req->in.h.opcode = CUSE_INIT;
	req->in.numargs = 1;
	req->in.args[0].size = sizeof(struct cuse_init_in);
	req->in.args[0].value = arg;
	req->out.numargs = 2;
	req->out.args[0].size = sizeof(struct cuse_init_out);
	req->out.args[0].value = outarg;
	req->out.args[1].size = CUSE_INIT_INFO_MAX;
	req->out.argvar = 1;
	req->out.argpages = 1;
	req->pages[0] = page;
	req->page_descs[0].length = req->out.args[1].size;
	req->num_pages = 1;
	req->end = cuse_process_init_reply;
	fuse_request_send_background(fc, req);

	return 0;

err_free_page:
	__free_page(page);
err_put_req:
	fuse_put_request(fc, req);
err:
	return rc;
}
