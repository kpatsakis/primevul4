static int tcmu_glfs_flush(struct tcmu_device *dev,
                           struct tcmulib_cmd *cmd)
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
	cookie->length = 0;
	cookie->op = TCMU_GLFS_FLUSH;

	if (glfs_fdatasync_async(state->gfd, glfs_async_cbk, cookie) < 0) {
		tcmu_err("glfs_fdatasync_async failed: %m\n");
		goto out;
	}

	return 0;

out:
	free(cookie);
	return SAM_STAT_TASK_SET_FULL;
}
