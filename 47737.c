pipe_release(struct inode *inode, struct file *file)
{
	struct pipe_inode_info *pipe = file->private_data;

	__pipe_lock(pipe);
	if (file->f_mode & FMODE_READ)
		pipe->readers--;
	if (file->f_mode & FMODE_WRITE)
		pipe->writers--;

	if (pipe->readers || pipe->writers) {
		wake_up_interruptible_sync_poll(&pipe->wait, POLLIN | POLLOUT | POLLRDNORM | POLLWRNORM | POLLERR | POLLHUP);
		kill_fasync(&pipe->fasync_readers, SIGIO, POLL_IN);
		kill_fasync(&pipe->fasync_writers, SIGIO, POLL_OUT);
	}
	__pipe_unlock(pipe);

	put_pipe_info(inode, pipe);
	return 0;
}
