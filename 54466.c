static void glfs_async_cbk(glfs_fd_t *fd, ssize_t ret, void *data)
{
	glfs_cbk_cookie *cookie = data;
	struct tcmu_device *dev = cookie->dev;
	struct tcmulib_cmd *cmd = cookie->cmd;
	size_t length = cookie->length;

	if (ret < 0 || ret != length) {
		/* Read/write/flush failed */
		switch (cookie->op) {
		case TCMU_GLFS_READ:
			ret =  tcmu_set_sense_data(cmd->sense_buf, MEDIUM_ERROR,
			                           ASC_READ_ERROR, NULL);
			break;
		case TCMU_GLFS_WRITE:
		case TCMU_GLFS_FLUSH:
			ret =  tcmu_set_sense_data(cmd->sense_buf, MEDIUM_ERROR,
			                           ASC_WRITE_ERROR, NULL);
			break;
		}
	} else {
		ret = SAM_STAT_GOOD;
	}

	cmd->done(dev, cmd, ret);
	free(cookie);
}
