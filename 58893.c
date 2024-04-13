int kernel_recvmsg(struct socket *sock, struct msghdr *msg,
		   struct kvec *vec, size_t num, size_t size, int flags)
{
	mm_segment_t oldfs = get_fs();
	int result;

	iov_iter_kvec(&msg->msg_iter, READ | ITER_KVEC, vec, num, size);
	set_fs(KERNEL_DS);
	result = sock_recvmsg(sock, msg, flags);
	set_fs(oldfs);
	return result;
}
