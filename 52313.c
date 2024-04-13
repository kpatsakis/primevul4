int usbip_recv(struct socket *sock, void *buf, int size)
{
	int result;
	struct msghdr msg;
	struct kvec iov;
	int total = 0;

	/* for blocks of if (usbip_dbg_flag_xmit) */
	char *bp = buf;
	int osize = size;

	usbip_dbg_xmit("enter\n");

	if (!sock || !buf || !size) {
		pr_err("invalid arg, sock %p buff %p size %d\n", sock, buf,
		       size);
		return -EINVAL;
	}

	do {
		sock->sk->sk_allocation = GFP_NOIO;
		iov.iov_base    = buf;
		iov.iov_len     = size;
		msg.msg_name    = NULL;
		msg.msg_namelen = 0;
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		msg.msg_flags      = MSG_NOSIGNAL;

		result = kernel_recvmsg(sock, &msg, &iov, 1, size, MSG_WAITALL);
		if (result <= 0) {
			pr_debug("receive sock %p buf %p size %u ret %d total %d\n",
				 sock, buf, size, result, total);
			goto err;
		}

		size -= result;
		buf += result;
		total += result;
	} while (size > 0);

	if (usbip_dbg_flag_xmit) {
		if (!in_interrupt())
			pr_debug("%-10s:", current->comm);
		else
			pr_debug("interrupt  :");

		pr_debug("receiving....\n");
		usbip_dump_buffer(bp, osize);
		pr_debug("received, osize %d ret %d size %d total %d\n",
			 osize, result, size, total);
	}

	return total;

err:
	return result;
}
