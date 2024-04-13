static long bsg_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct bsg_device *bd = file->private_data;
	int __user *uarg = (int __user *) arg;
	int ret;

	switch (cmd) {
		/*
		 * our own ioctls
		 */
	case SG_GET_COMMAND_Q:
		return put_user(bd->max_queue, uarg);
	case SG_SET_COMMAND_Q: {
		int queue;

		if (get_user(queue, uarg))
			return -EFAULT;
		if (queue < 1)
			return -EINVAL;

		spin_lock_irq(&bd->lock);
		bd->max_queue = queue;
		spin_unlock_irq(&bd->lock);
		return 0;
	}

	/*
	 * SCSI/sg ioctls
	 */
	case SG_GET_VERSION_NUM:
	case SCSI_IOCTL_GET_IDLUN:
	case SCSI_IOCTL_GET_BUS_NUMBER:
	case SG_SET_TIMEOUT:
	case SG_GET_TIMEOUT:
	case SG_GET_RESERVED_SIZE:
	case SG_SET_RESERVED_SIZE:
	case SG_EMULATED_HOST:
	case SCSI_IOCTL_SEND_COMMAND: {
		void __user *uarg = (void __user *) arg;
		return scsi_cmd_ioctl(bd->queue, NULL, file->f_mode, cmd, uarg);
	}
	case SG_IO: {
		struct request *rq;
		struct bio *bio, *bidi_bio = NULL;
		struct sg_io_v4 hdr;
		int at_head;
		u8 sense[SCSI_SENSE_BUFFERSIZE];

		if (copy_from_user(&hdr, uarg, sizeof(hdr)))
			return -EFAULT;

		rq = bsg_map_hdr(bd, &hdr, file->f_mode & FMODE_WRITE, sense);
		if (IS_ERR(rq))
			return PTR_ERR(rq);

		bio = rq->bio;
		if (rq->next_rq)
			bidi_bio = rq->next_rq->bio;

		at_head = (0 == (hdr.flags & BSG_FLAG_Q_AT_TAIL));
		blk_execute_rq(bd->queue, NULL, rq, at_head);
		ret = blk_complete_sgv4_hdr_rq(rq, &hdr, bio, bidi_bio);

		if (copy_to_user(uarg, &hdr, sizeof(hdr)))
			return -EFAULT;

		return ret;
	}
	/*
	 * block device ioctls
	 */
	default:
#if 0
		return ioctl_by_bdev(bd->bdev, cmd, arg);
#else
		return -ENOTTY;
#endif
	}
}
