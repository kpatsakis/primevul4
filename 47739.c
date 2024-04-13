void pipe_wait(struct pipe_inode_info *pipe)
{
	DEFINE_WAIT(wait);

	/*
	 * Pipes are system-local resources, so sleeping on them
	 * is considered a noninteractive wait:
	 */
	prepare_to_wait(&pipe->wait, &wait, TASK_INTERRUPTIBLE);
	pipe_unlock(pipe);
	schedule();
	finish_wait(&pipe->wait, &wait);
 	pipe_lock(pipe);
 }
