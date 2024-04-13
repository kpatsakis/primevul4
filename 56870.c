static unsigned int mqueue_poll_file(struct file *filp, struct poll_table_struct *poll_tab)
{
	struct mqueue_inode_info *info = MQUEUE_I(file_inode(filp));
	int retval = 0;

	poll_wait(filp, &info->wait_q, poll_tab);

	spin_lock(&info->lock);
	if (info->attr.mq_curmsgs)
		retval = POLLIN | POLLRDNORM;

	if (info->attr.mq_curmsgs < info->attr.mq_maxmsg)
		retval |= POLLOUT | POLLWRNORM;
	spin_unlock(&info->lock);

	return retval;
}
