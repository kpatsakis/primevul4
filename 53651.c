static size_t fuse_async_req_send(struct fuse_conn *fc, struct fuse_req *req,
		size_t num_bytes, struct fuse_io_priv *io)
{
	spin_lock(&io->lock);
	io->size += num_bytes;
	io->reqs++;
	spin_unlock(&io->lock);

	req->io = io;
	req->end = fuse_aio_complete_req;

	__fuse_get_request(req);
	fuse_request_send_background(fc, req);

	return num_bytes;
}
