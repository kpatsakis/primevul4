static int ipipe_prep(struct pipe_inode_info *pipe, unsigned int flags)
{
	int ret;

	/*
	 * Check ->nrbufs without the inode lock first. This function
	 * is speculative anyways, so missing one is ok.
	 */
	if (pipe->nrbufs)
		return 0;

	ret = 0;
	pipe_lock(pipe);

	while (!pipe->nrbufs) {
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			break;
		}
		if (!pipe->writers)
			break;
		if (!pipe->waiting_writers) {
			if (flags & SPLICE_F_NONBLOCK) {
				ret = -EAGAIN;
				break;
			}
		}
		pipe_wait(pipe);
	}

	pipe_unlock(pipe);
	return ret;
}
