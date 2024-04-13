void __audit_fd_pair(int fd1, int fd2)
{
	struct audit_context *context = current->audit_context;
	context->fds[0] = fd1;
	context->fds[1] = fd2;
}
