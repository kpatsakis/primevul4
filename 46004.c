static int do_cmd_ioctl(struct comedi_device *dev,
			struct comedi_cmd __user *cmd, void *file)
{
	struct comedi_cmd user_cmd;
	struct comedi_subdevice *s;
	struct comedi_async *async;
	int ret = 0;
	unsigned int __user *chanlist_saver = NULL;

	if (copy_from_user(&user_cmd, cmd, sizeof(struct comedi_cmd))) {
		DPRINTK("bad cmd address\n");
		return -EFAULT;
	}
	/* save user's chanlist pointer so it can be restored later */
	chanlist_saver = user_cmd.chanlist;

	if (user_cmd.subdev >= dev->n_subdevices) {
		DPRINTK("%d no such subdevice\n", user_cmd.subdev);
		return -ENODEV;
	}

	s = dev->subdevices + user_cmd.subdev;
	async = s->async;

	if (s->type == COMEDI_SUBD_UNUSED) {
		DPRINTK("%d not valid subdevice\n", user_cmd.subdev);
		return -EIO;
	}

	if (!s->do_cmd || !s->do_cmdtest || !s->async) {
		DPRINTK("subdevice %i does not support commands\n",
			user_cmd.subdev);
		return -EIO;
	}

	/* are we locked? (ioctl lock) */
	if (s->lock && s->lock != file) {
		DPRINTK("subdevice locked\n");
		return -EACCES;
	}

	/* are we busy? */
	if (s->busy) {
		DPRINTK("subdevice busy\n");
		return -EBUSY;
	}
	s->busy = file;

	/* make sure channel/gain list isn't too long */
	if (user_cmd.chanlist_len > s->len_chanlist) {
		DPRINTK("channel/gain list too long %u > %d\n",
			user_cmd.chanlist_len, s->len_chanlist);
		ret = -EINVAL;
		goto cleanup;
	}

	/* make sure channel/gain list isn't too short */
	if (user_cmd.chanlist_len < 1) {
		DPRINTK("channel/gain list too short %u < 1\n",
			user_cmd.chanlist_len);
		ret = -EINVAL;
		goto cleanup;
	}

	kfree(async->cmd.chanlist);
	async->cmd = user_cmd;
	async->cmd.data = NULL;
	/* load channel/gain list */
	async->cmd.chanlist =
	    kmalloc(async->cmd.chanlist_len * sizeof(int), GFP_KERNEL);
	if (!async->cmd.chanlist) {
		DPRINTK("allocation failed\n");
		ret = -ENOMEM;
		goto cleanup;
	}

	if (copy_from_user(async->cmd.chanlist, user_cmd.chanlist,
			   async->cmd.chanlist_len * sizeof(int))) {
		DPRINTK("fault reading chanlist\n");
		ret = -EFAULT;
		goto cleanup;
	}

	/* make sure each element in channel/gain list is valid */
	ret = comedi_check_chanlist(s,
				    async->cmd.chanlist_len,
				    async->cmd.chanlist);
	if (ret < 0) {
		DPRINTK("bad chanlist\n");
		goto cleanup;
	}

	ret = s->do_cmdtest(dev, s, &async->cmd);

	if (async->cmd.flags & TRIG_BOGUS || ret) {
		DPRINTK("test returned %d\n", ret);
		user_cmd = async->cmd;
		/* restore chanlist pointer before copying back */
		user_cmd.chanlist = chanlist_saver;
		user_cmd.data = NULL;
		if (copy_to_user(cmd, &user_cmd, sizeof(struct comedi_cmd))) {
			DPRINTK("fault writing cmd\n");
			ret = -EFAULT;
			goto cleanup;
		}
		ret = -EAGAIN;
		goto cleanup;
	}

	if (!async->prealloc_bufsz) {
		ret = -ENOMEM;
		DPRINTK("no buffer (?)\n");
		goto cleanup;
	}

	comedi_reset_async_buf(async);

	async->cb_mask =
	    COMEDI_CB_EOA | COMEDI_CB_BLOCK | COMEDI_CB_ERROR |
	    COMEDI_CB_OVERFLOW;
	if (async->cmd.flags & TRIG_WAKE_EOS)
		async->cb_mask |= COMEDI_CB_EOS;

	comedi_set_subdevice_runflags(s, ~0, SRF_USER | SRF_RUNNING);

	ret = s->do_cmd(dev, s);
	if (ret == 0)
		return 0;

cleanup:
	do_become_nonbusy(dev, s);

	return ret;
}
