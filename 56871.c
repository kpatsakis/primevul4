static ssize_t mqueue_read_file(struct file *filp, char __user *u_data,
				size_t count, loff_t *off)
{
	struct mqueue_inode_info *info = MQUEUE_I(file_inode(filp));
	char buffer[FILENT_SIZE];
	ssize_t ret;

	spin_lock(&info->lock);
	snprintf(buffer, sizeof(buffer),
			"QSIZE:%-10lu NOTIFY:%-5d SIGNO:%-5d NOTIFY_PID:%-6d\n",
			info->qsize,
			info->notify_owner ? info->notify.sigev_notify : 0,
			(info->notify_owner &&
			 info->notify.sigev_notify == SIGEV_SIGNAL) ?
				info->notify.sigev_signo : 0,
			pid_vnr(info->notify_owner));
	spin_unlock(&info->lock);
	buffer[sizeof(buffer)-1] = '\0';

	ret = simple_read_from_buffer(u_data, count, off, buffer,
				strlen(buffer));
	if (ret <= 0)
		return ret;

	file_inode(filp)->i_atime = file_inode(filp)->i_ctime = current_time(file_inode(filp));
	return ret;
}
