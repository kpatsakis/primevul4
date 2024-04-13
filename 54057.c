static struct cuse_conn *fc_to_cc(struct fuse_conn *fc)
{
	return container_of(fc, struct cuse_conn, fc);
}
