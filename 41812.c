xfs_buf_free_maps(
	struct xfs_buf	*bp)
{
	if (bp->b_maps != &bp->__b_map) {
		kmem_free(bp->b_maps);
		bp->b_maps = NULL;
	}
}
