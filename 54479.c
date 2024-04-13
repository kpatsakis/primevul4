static int tcmu_glfs_read(struct tcmu_device *dev,
                          struct tcmulib_cmd *cmd,
                          struct iovec *iov, size_t iov_cnt,
                          size_t length, off_t offset)
{
	struct glfs_state *state = tcmu_get_dev_private(dev);
	glfs_cbk_cookie *cookie;

	cookie = calloc(1, sizeof(*cookie));
	if (!cookie) {
		tcmu_err("Could not allocate cookie: %m\n");
		goto out;
	}
	cookie->dev = dev;
	cookie->cmd = cmd;
	cookie->length = length;
	cookie->op = TCMU_GLFS_READ;

	if (glfs_preadv_async(state->gfd, iov, iov_cnt, offset, SEEK_SET,
	                      glfs_async_cbk, cookie) < 0) {
		tcmu_err("glfs_preadv_async failed: %m\n");
		goto out;
	}

	return 0;

out:
	free(cookie);
	return SAM_STAT_TASK_SET_FULL;
}
