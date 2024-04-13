static void *copy_array(void *dst, const void *src, size_t n, size_t size, gfp_t flags)
{
	size_t bytes;

	if (ZERO_OR_NULL_PTR(src))
		goto out;

	if (unlikely(check_mul_overflow(n, size, &bytes)))
		return NULL;

	if (ksize(dst) < bytes) {
		kfree(dst);
		dst = kmalloc_track_caller(bytes, flags);
		if (!dst)
			return NULL;
	}

	memcpy(dst, src, bytes);
out:
	return dst ? dst : ZERO_SIZE_PTR;
}