static inline int err_block_err(int ret)
{
	if (ret && ret != -ENOSPC && ret != -ENODATA && ret != -EAGAIN)
		return 1;

	return 0;
}
