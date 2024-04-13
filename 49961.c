__bsg_read(char __user *buf, size_t count, struct bsg_device *bd,
	   const struct iovec *iov, ssize_t *bytes_read)
{
	struct bsg_command *bc;
	int nr_commands, ret;

	if (count % sizeof(struct sg_io_v4))
		return -EINVAL;

	ret = 0;
	nr_commands = count / sizeof(struct sg_io_v4);
	while (nr_commands) {
		bc = bsg_get_done_cmd(bd);
		if (IS_ERR(bc)) {
			ret = PTR_ERR(bc);
			break;
		}

		/*
		 * this is the only case where we need to copy data back
		 * after completing the request. so do that here,
		 * bsg_complete_work() cannot do that for us
		 */
		ret = blk_complete_sgv4_hdr_rq(bc->rq, &bc->hdr, bc->bio,
					       bc->bidi_bio);

		if (copy_to_user(buf, &bc->hdr, sizeof(bc->hdr)))
			ret = -EFAULT;

		bsg_free_command(bc);

		if (ret)
			break;

		buf += sizeof(struct sg_io_v4);
		*bytes_read += sizeof(struct sg_io_v4);
		nr_commands--;
	}

	return ret;
}
