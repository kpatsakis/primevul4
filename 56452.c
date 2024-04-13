static struct bio_map_data *bio_alloc_map_data(unsigned int iov_count,
					       gfp_t gfp_mask)
{
	if (iov_count > UIO_MAXIOV)
		return NULL;

	return kmalloc(sizeof(struct bio_map_data) +
		       sizeof(struct iovec) * iov_count, gfp_mask);
}
