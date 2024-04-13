static int do_mq_getsetattr(int mqdes, struct mq_attr *new, struct mq_attr *old)
{
	struct fd f;
	struct inode *inode;
	struct mqueue_inode_info *info;

	if (new && (new->mq_flags & (~O_NONBLOCK)))
		return -EINVAL;

	f = fdget(mqdes);
	if (!f.file)
		return -EBADF;

	if (unlikely(f.file->f_op != &mqueue_file_operations)) {
		fdput(f);
		return -EBADF;
	}

	inode = file_inode(f.file);
	info = MQUEUE_I(inode);

	spin_lock(&info->lock);

	if (old) {
		*old = info->attr;
		old->mq_flags = f.file->f_flags & O_NONBLOCK;
	}
	if (new) {
		audit_mq_getsetattr(mqdes, new);
		spin_lock(&f.file->f_lock);
		if (new->mq_flags & O_NONBLOCK)
			f.file->f_flags |= O_NONBLOCK;
		else
			f.file->f_flags &= ~O_NONBLOCK;
		spin_unlock(&f.file->f_lock);

		inode->i_atime = inode->i_ctime = current_time(inode);
	}

	spin_unlock(&info->lock);
	fdput(f);
	return 0;
}
