static int snd_seq_ioctl_get_client_pool(struct snd_seq_client *client,
					 void __user *arg)
{
	struct snd_seq_client_pool info;
	struct snd_seq_client *cptr;

	if (copy_from_user(&info, arg, sizeof(info)))
		return -EFAULT;

	cptr = snd_seq_client_use_ptr(info.client);
	if (cptr == NULL)
		return -ENOENT;
	memset(&info, 0, sizeof(info));
	info.client = cptr->number;
	info.output_pool = cptr->pool->size;
	info.output_room = cptr->pool->room;
	info.output_free = info.output_pool;
	info.output_free = snd_seq_unused_cells(cptr->pool);
	if (cptr->type == USER_CLIENT) {
		info.input_pool = cptr->data.user.fifo_pool_size;
		info.input_free = info.input_pool;
		if (cptr->data.user.fifo)
			info.input_free = snd_seq_unused_cells(cptr->data.user.fifo->pool);
	} else {
		info.input_pool = 0;
		info.input_free = 0;
	}
	snd_seq_client_unlock(cptr);
	
	if (copy_to_user(arg, &info, sizeof(info)))
		return -EFAULT;
	return 0;
}
