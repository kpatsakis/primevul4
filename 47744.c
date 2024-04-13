static int wait_for_partner(struct pipe_inode_info *pipe, unsigned int *cnt)
{
	int cur = *cnt;	

	while (cur == *cnt) {
		pipe_wait(pipe);
		if (signal_pending(current))
			break;
	}
	return cur == *cnt ? -ERESTARTSYS : 0;
}
