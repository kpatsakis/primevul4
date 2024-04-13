static int digest_update_interruptible(struct digest *d, const void *data,
				       unsigned long len)
{
	if (ctrlc())
		return -EINTR;

	return digest_update(d, data, len);
}