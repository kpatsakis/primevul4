static int __tipc_sendmsg(struct socket *sock, struct msghdr *m, size_t dsz)
{
	DECLARE_SOCKADDR(struct sockaddr_tipc *, dest, m->msg_name);
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct net *net = sock_net(sk);
	struct tipc_msg *mhdr = &tsk->phdr;
	u32 dnode, dport;
	struct sk_buff_head pktchain;
	struct sk_buff *skb;
	struct tipc_name_seq *seq;
	struct iov_iter save;
	u32 mtu;
	long timeo;
	int rc;

	if (dsz > TIPC_MAX_USER_MSG_SIZE)
		return -EMSGSIZE;
	if (unlikely(!dest)) {
		if (tsk->connected && sock->state == SS_READY)
			dest = &tsk->remote;
		else
			return -EDESTADDRREQ;
	} else if (unlikely(m->msg_namelen < sizeof(*dest)) ||
		   dest->family != AF_TIPC) {
		return -EINVAL;
	}
	if (unlikely(sock->state != SS_READY)) {
		if (sock->state == SS_LISTENING)
			return -EPIPE;
		if (sock->state != SS_UNCONNECTED)
			return -EISCONN;
		if (tsk->published)
			return -EOPNOTSUPP;
		if (dest->addrtype == TIPC_ADDR_NAME) {
			tsk->conn_type = dest->addr.name.name.type;
			tsk->conn_instance = dest->addr.name.name.instance;
		}
	}
	seq = &dest->addr.nameseq;
	timeo = sock_sndtimeo(sk, m->msg_flags & MSG_DONTWAIT);

	if (dest->addrtype == TIPC_ADDR_MCAST) {
		return tipc_sendmcast(sock, seq, m, dsz, timeo);
	} else if (dest->addrtype == TIPC_ADDR_NAME) {
		u32 type = dest->addr.name.name.type;
		u32 inst = dest->addr.name.name.instance;
		u32 domain = dest->addr.name.domain;

		dnode = domain;
		msg_set_type(mhdr, TIPC_NAMED_MSG);
		msg_set_hdr_sz(mhdr, NAMED_H_SIZE);
		msg_set_nametype(mhdr, type);
		msg_set_nameinst(mhdr, inst);
		msg_set_lookup_scope(mhdr, tipc_addr_scope(domain));
		dport = tipc_nametbl_translate(net, type, inst, &dnode);
		msg_set_destnode(mhdr, dnode);
		msg_set_destport(mhdr, dport);
		if (unlikely(!dport && !dnode))
			return -EHOSTUNREACH;
	} else if (dest->addrtype == TIPC_ADDR_ID) {
		dnode = dest->addr.id.node;
		msg_set_type(mhdr, TIPC_DIRECT_MSG);
		msg_set_lookup_scope(mhdr, 0);
		msg_set_destnode(mhdr, dnode);
		msg_set_destport(mhdr, dest->addr.id.ref);
		msg_set_hdr_sz(mhdr, BASIC_H_SIZE);
	}

	skb_queue_head_init(&pktchain);
	save = m->msg_iter;
new_mtu:
	mtu = tipc_node_get_mtu(net, dnode, tsk->portid);
	rc = tipc_msg_build(mhdr, m, 0, dsz, mtu, &pktchain);
	if (rc < 0)
		return rc;

	do {
		skb = skb_peek(&pktchain);
		TIPC_SKB_CB(skb)->wakeup_pending = tsk->link_cong;
		rc = tipc_node_xmit(net, &pktchain, dnode, tsk->portid);
		if (likely(!rc)) {
			if (sock->state != SS_READY)
				sock->state = SS_CONNECTING;
			return dsz;
		}
		if (rc == -ELINKCONG) {
			tsk->link_cong = 1;
			rc = tipc_wait_for_sndmsg(sock, &timeo);
			if (!rc)
				continue;
		}
		__skb_queue_purge(&pktchain);
		if (rc == -EMSGSIZE) {
			m->msg_iter = save;
			goto new_mtu;
		}
		break;
	} while (1);

	return rc;
}
