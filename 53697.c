static void fuse_release_end(struct fuse_conn *fc, struct fuse_req *req)
{
	iput(req->misc.release.inode);
}
