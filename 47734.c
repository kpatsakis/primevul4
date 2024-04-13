pipe_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask;
	struct pipe_inode_info *pipe = filp->private_data;
	int nrbufs;

	poll_wait(filp, &pipe->wait, wait);

	/* Reading only -- no need for acquiring the semaphore.  */
	nrbufs = pipe->nrbufs;
	mask = 0;
	if (filp->f_mode & FMODE_READ) {
		mask = (nrbufs > 0) ? POLLIN | POLLRDNORM : 0;
		if (!pipe->writers && filp->f_version != pipe->w_counter)
			mask |= POLLHUP;
	}

	if (filp->f_mode & FMODE_WRITE) {
		mask |= (nrbufs < pipe->buffers) ? POLLOUT | POLLWRNORM : 0;
		/*
		 * Most Unices do not set POLLERR for FIFOs but on Linux they
		 * behave exactly like pipes for poll().
		 */
		if (!pipe->readers)
			mask |= POLLERR;
	}

	return mask;
}
