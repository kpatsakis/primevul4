static int do_mq_timedsend(mqd_t mqdes, const char __user *u_msg_ptr,
		size_t msg_len, unsigned int msg_prio,
		struct timespec *ts)
{
	struct fd f;
	struct inode *inode;
	struct ext_wait_queue wait;
	struct ext_wait_queue *receiver;
	struct msg_msg *msg_ptr;
	struct mqueue_inode_info *info;
	ktime_t expires, *timeout = NULL;
	struct posix_msg_tree_node *new_leaf = NULL;
	int ret = 0;
	DEFINE_WAKE_Q(wake_q);

	if (unlikely(msg_prio >= (unsigned long) MQ_PRIO_MAX))
		return -EINVAL;

	if (ts) {
		expires = timespec_to_ktime(*ts);
		timeout = &expires;
	}

	audit_mq_sendrecv(mqdes, msg_len, msg_prio, ts);

	f = fdget(mqdes);
	if (unlikely(!f.file)) {
		ret = -EBADF;
		goto out;
	}

	inode = file_inode(f.file);
	if (unlikely(f.file->f_op != &mqueue_file_operations)) {
		ret = -EBADF;
		goto out_fput;
	}
	info = MQUEUE_I(inode);
	audit_file(f.file);

	if (unlikely(!(f.file->f_mode & FMODE_WRITE))) {
		ret = -EBADF;
		goto out_fput;
	}

	if (unlikely(msg_len > info->attr.mq_msgsize)) {
		ret = -EMSGSIZE;
		goto out_fput;
	}

	/* First try to allocate memory, before doing anything with
	 * existing queues. */
	msg_ptr = load_msg(u_msg_ptr, msg_len);
	if (IS_ERR(msg_ptr)) {
		ret = PTR_ERR(msg_ptr);
		goto out_fput;
	}
	msg_ptr->m_ts = msg_len;
	msg_ptr->m_type = msg_prio;

	/*
	 * msg_insert really wants us to have a valid, spare node struct so
	 * it doesn't have to kmalloc a GFP_ATOMIC allocation, but it will
	 * fall back to that if necessary.
	 */
	if (!info->node_cache)
		new_leaf = kmalloc(sizeof(*new_leaf), GFP_KERNEL);

	spin_lock(&info->lock);

	if (!info->node_cache && new_leaf) {
		/* Save our speculative allocation into the cache */
		INIT_LIST_HEAD(&new_leaf->msg_list);
		info->node_cache = new_leaf;
		new_leaf = NULL;
	} else {
		kfree(new_leaf);
	}

	if (info->attr.mq_curmsgs == info->attr.mq_maxmsg) {
		if (f.file->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
		} else {
			wait.task = current;
			wait.msg = (void *) msg_ptr;
			wait.state = STATE_NONE;
			ret = wq_sleep(info, SEND, timeout, &wait);
			/*
			 * wq_sleep must be called with info->lock held, and
			 * returns with the lock released
			 */
			goto out_free;
		}
	} else {
		receiver = wq_get_first_waiter(info, RECV);
		if (receiver) {
			pipelined_send(&wake_q, info, msg_ptr, receiver);
		} else {
			/* adds message to the queue */
			ret = msg_insert(msg_ptr, info);
			if (ret)
				goto out_unlock;
			__do_notify(info);
		}
		inode->i_atime = inode->i_mtime = inode->i_ctime =
				current_time(inode);
	}
out_unlock:
	spin_unlock(&info->lock);
	wake_up_q(&wake_q);
out_free:
	if (ret)
		free_msg(msg_ptr);
out_fput:
	fdput(f);
out:
	return ret;
}
