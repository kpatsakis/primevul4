static int __tipc_send_stream(struct socket *sock, struct msghdr *m, size_t dsz)
{
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *mhdr = &tsk->phdr;
	struct sk_buff_head pktchain;
	DECLARE_SOCKADDR(struct sockaddr_tipc *, dest, m->msg_name);
	u32 portid = tsk->portid;
	int rc = -EINVAL;
	long timeo;
	u32 dnode;
	uint mtu, send, sent = 0;
	struct iov_iter save;
	int hlen = MIN_H_SIZE;

	/* Handle implied connection establishment */
	if (unlikely(dest)) {
		rc = __tipc_sendmsg(sock, m, dsz);
		hlen = msg_hdr_sz(mhdr);
		if (dsz && (dsz == rc))
			tsk->snt_unacked = tsk_inc(tsk, dsz + hlen);
		return rc;
	}
	if (dsz > (uint)INT_MAX)
		return -EMSGSIZE;

	if (unlikely(sock->state != SS_CONNECTED)) {
		if (sock->state == SS_DISCONNECTING)
			return -EPIPE;
		else
			return -ENOTCONN;
	}

	timeo = sock_sndtimeo(sk, m->msg_flags & MSG_DONTWAIT);
	dnode = tsk_peer_node(tsk);
	skb_queue_head_init(&pktchain);

next:
	save = m->msg_iter;
	mtu = tsk->max_pkt;
	send = min_t(uint, dsz - sent, TIPC_MAX_USER_MSG_SIZE);
	rc = tipc_msg_build(mhdr, m, sent, send, mtu, &pktchain);
	if (unlikely(rc < 0))
		return rc;

	do {
		if (likely(!tsk_conn_cong(tsk))) {
			rc = tipc_node_xmit(net, &pktchain, dnode, portid);
			if (likely(!rc)) {
				tsk->snt_unacked += tsk_inc(tsk, send + hlen);
				sent += send;
				if (sent == dsz)
					return dsz;
				goto next;
			}
			if (rc == -EMSGSIZE) {
				__skb_queue_purge(&pktchain);
				tsk->max_pkt = tipc_node_get_mtu(net, dnode,
								 portid);
				m->msg_iter = save;
				goto next;
			}
			if (rc != -ELINKCONG)
				break;

			tsk->link_cong = 1;
		}
		rc = tipc_wait_for_sndpkt(sock, &timeo);
	} while (!rc);

	__skb_queue_purge(&pktchain);
	return sent ? sent : rc;
}
