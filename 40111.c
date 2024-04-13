int vcc_sendmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
		size_t total_len)
{
	struct sock *sk = sock->sk;
	DEFINE_WAIT(wait);
	struct atm_vcc *vcc;
	struct sk_buff *skb;
	int eff, error;
	const void __user *buff;
	int size;

	lock_sock(sk);
	if (sock->state != SS_CONNECTED) {
		error = -ENOTCONN;
		goto out;
	}
	if (m->msg_name) {
		error = -EISCONN;
		goto out;
	}
	if (m->msg_iovlen != 1) {
		error = -ENOSYS; /* fix this later @@@ */
		goto out;
	}
	buff = m->msg_iov->iov_base;
	size = m->msg_iov->iov_len;
	vcc = ATM_SD(sock);
	if (test_bit(ATM_VF_RELEASED, &vcc->flags) ||
	    test_bit(ATM_VF_CLOSE, &vcc->flags) ||
	    !test_bit(ATM_VF_READY, &vcc->flags)) {
		error = -EPIPE;
		send_sig(SIGPIPE, current, 0);
		goto out;
	}
	if (!size) {
		error = 0;
		goto out;
	}
	if (size < 0 || size > vcc->qos.txtp.max_sdu) {
		error = -EMSGSIZE;
		goto out;
	}

	eff = (size+3) & ~3; /* align to word boundary */
	prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
	error = 0;
	while (!(skb = alloc_tx(vcc, eff))) {
		if (m->msg_flags & MSG_DONTWAIT) {
			error = -EAGAIN;
			break;
		}
		schedule();
		if (signal_pending(current)) {
			error = -ERESTARTSYS;
			break;
		}
		if (test_bit(ATM_VF_RELEASED, &vcc->flags) ||
		    test_bit(ATM_VF_CLOSE, &vcc->flags) ||
		    !test_bit(ATM_VF_READY, &vcc->flags)) {
			error = -EPIPE;
			send_sig(SIGPIPE, current, 0);
			break;
		}
		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
	}
	finish_wait(sk_sleep(sk), &wait);
	if (error)
		goto out;
	skb->dev = NULL; /* for paths shared with net_device interfaces */
	ATM_SKB(skb)->atm_options = vcc->atm_options;
	if (copy_from_user(skb_put(skb, size), buff, size)) {
		kfree_skb(skb);
		error = -EFAULT;
		goto out;
	}
	if (eff != size)
		memset(skb->data + size, 0, eff-size);
	error = vcc->dev->ops->send(vcc, skb);
	error = error ? error : size;
out:
	release_sock(sk);
	return error;
}
