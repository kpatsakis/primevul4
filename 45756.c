static int unix_dgram_sendmsg(struct kiocb *kiocb, struct socket *sock,
			      struct msghdr *msg, size_t len)
{
	struct sock_iocb *siocb = kiocb_to_siocb(kiocb);
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct unix_sock *u = unix_sk(sk);
	struct sockaddr_un *sunaddr = msg->msg_name;
	struct sock *other = NULL;
	int namelen = 0; /* fake GCC */
	int err;
	unsigned int hash;
	struct sk_buff *skb;
	long timeo;
	struct scm_cookie tmp_scm;
	int max_level;
	int data_len = 0;

	if (NULL == siocb->scm)
		siocb->scm = &tmp_scm;
	wait_for_unix_gc();
	err = scm_send(sock, msg, siocb->scm, false);
	if (err < 0)
		return err;

	err = -EOPNOTSUPP;
	if (msg->msg_flags&MSG_OOB)
		goto out;

	if (msg->msg_namelen) {
		err = unix_mkname(sunaddr, msg->msg_namelen, &hash);
		if (err < 0)
			goto out;
		namelen = err;
	} else {
		sunaddr = NULL;
		err = -ENOTCONN;
		other = unix_peer_get(sk);
		if (!other)
			goto out;
	}

	if (test_bit(SOCK_PASSCRED, &sock->flags) && !u->addr
	    && (err = unix_autobind(sock)) != 0)
		goto out;

	err = -EMSGSIZE;
	if (len > sk->sk_sndbuf - 32)
		goto out;

	if (len > SKB_MAX_ALLOC)
		data_len = min_t(size_t,
				 len - SKB_MAX_ALLOC,
				 MAX_SKB_FRAGS * PAGE_SIZE);

	skb = sock_alloc_send_pskb(sk, len - data_len, data_len,
				   msg->msg_flags & MSG_DONTWAIT, &err,
				   PAGE_ALLOC_COSTLY_ORDER);
	if (skb == NULL)
		goto out;

	err = unix_scm_to_skb(siocb->scm, skb, true);
	if (err < 0)
		goto out_free;
	max_level = err + 1;
	unix_get_secdata(siocb->scm, skb);

	skb_put(skb, len - data_len);
	skb->data_len = data_len;
	skb->len = len;
	err = skb_copy_datagram_from_iovec(skb, 0, msg->msg_iov, 0, len);
	if (err)
		goto out_free;

	timeo = sock_sndtimeo(sk, msg->msg_flags & MSG_DONTWAIT);

restart:
	if (!other) {
		err = -ECONNRESET;
		if (sunaddr == NULL)
			goto out_free;

		other = unix_find_other(net, sunaddr, namelen, sk->sk_type,
					hash, &err);
		if (other == NULL)
			goto out_free;
	}

	if (sk_filter(other, skb) < 0) {
		/* Toss the packet but do not return any error to the sender */
		err = len;
		goto out_free;
	}

	unix_state_lock(other);
	err = -EPERM;
	if (!unix_may_send(sk, other))
		goto out_unlock;

	if (sock_flag(other, SOCK_DEAD)) {
		/*
		 *	Check with 1003.1g - what should
		 *	datagram error
		 */
		unix_state_unlock(other);
		sock_put(other);

		err = 0;
		unix_state_lock(sk);
		if (unix_peer(sk) == other) {
			unix_peer(sk) = NULL;
			unix_state_unlock(sk);

			unix_dgram_disconnected(sk, other);
			sock_put(other);
			err = -ECONNREFUSED;
		} else {
			unix_state_unlock(sk);
		}

		other = NULL;
		if (err)
			goto out_free;
		goto restart;
	}

	err = -EPIPE;
	if (other->sk_shutdown & RCV_SHUTDOWN)
		goto out_unlock;

	if (sk->sk_type != SOCK_SEQPACKET) {
		err = security_unix_may_send(sk->sk_socket, other->sk_socket);
		if (err)
			goto out_unlock;
	}

	if (unix_peer(other) != sk && unix_recvq_full(other)) {
		if (!timeo) {
			err = -EAGAIN;
			goto out_unlock;
		}

		timeo = unix_wait_for_peer(other, timeo);

		err = sock_intr_errno(timeo);
		if (signal_pending(current))
			goto out_free;

		goto restart;
	}

	if (sock_flag(other, SOCK_RCVTSTAMP))
		__net_timestamp(skb);
	maybe_add_creds(skb, sock, other);
	skb_queue_tail(&other->sk_receive_queue, skb);
	if (max_level > unix_sk(other)->recursion_level)
		unix_sk(other)->recursion_level = max_level;
	unix_state_unlock(other);
	other->sk_data_ready(other, len);
	sock_put(other);
	scm_destroy(siocb->scm);
	return len;

out_unlock:
	unix_state_unlock(other);
out_free:
	kfree_skb(skb);
out:
	if (other)
		sock_put(other);
	scm_destroy(siocb->scm);
	return err;
}
