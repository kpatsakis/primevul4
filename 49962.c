static int __bsg_write(struct bsg_device *bd, const char __user *buf,
		       size_t count, ssize_t *bytes_written,
		       fmode_t has_write_perm)
{
	struct bsg_command *bc;
	struct request *rq;
	int ret, nr_commands;

	if (count % sizeof(struct sg_io_v4))
		return -EINVAL;

	nr_commands = count / sizeof(struct sg_io_v4);
	rq = NULL;
	bc = NULL;
	ret = 0;
	while (nr_commands) {
		struct request_queue *q = bd->queue;

		bc = bsg_alloc_command(bd);
		if (IS_ERR(bc)) {
			ret = PTR_ERR(bc);
			bc = NULL;
			break;
		}

		if (copy_from_user(&bc->hdr, buf, sizeof(bc->hdr))) {
			ret = -EFAULT;
			break;
		}

		/*
		 * get a request, fill in the blanks, and add to request queue
		 */
		rq = bsg_map_hdr(bd, &bc->hdr, has_write_perm, bc->sense);
		if (IS_ERR(rq)) {
			ret = PTR_ERR(rq);
			rq = NULL;
			break;
		}

		bsg_add_command(bd, q, bc, rq);
		bc = NULL;
		rq = NULL;
		nr_commands--;
		buf += sizeof(struct sg_io_v4);
		*bytes_written += sizeof(struct sg_io_v4);
	}

	if (bc)
		bsg_free_command(bc);

	return ret;
}
