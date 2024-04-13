static void nfs_context_set_write_error(struct nfs_open_context *ctx, int error)
{
	ctx->error = error;
	smp_wmb();
	set_bit(NFS_CONTEXT_ERROR_WRITE, &ctx->flags);
}
