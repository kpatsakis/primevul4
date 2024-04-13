int kernel_sendmsg(struct socket *sock, struct msghdr *msg,
		   struct kvec *vec, size_t num, size_t size)
{
	iov_iter_kvec(&msg->msg_iter, WRITE | ITER_KVEC, vec, num, size);
	return sock_sendmsg(sock, msg);
}
