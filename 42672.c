static int sctp_getsockopt_peer_addrs_old(struct sock *sk, int len,
					  char __user *optval,
					  int __user *optlen)
{
	struct sctp_association *asoc;
	struct list_head *pos;
	int cnt = 0;
	struct sctp_getaddrs_old getaddrs;
	struct sctp_transport *from;
	void __user *to;
	union sctp_addr temp;
	struct sctp_sock *sp = sctp_sk(sk);
	int addrlen;

	if (len != sizeof(struct sctp_getaddrs_old))
		return -EINVAL;

	if (copy_from_user(&getaddrs, optval, sizeof(struct sctp_getaddrs_old)))
		return -EFAULT;

	if (getaddrs.addr_num <= 0) return -EINVAL;

	/* For UDP-style sockets, id specifies the association to query.  */
	asoc = sctp_id2assoc(sk, getaddrs.assoc_id);
	if (!asoc)
		return -EINVAL;

	to = (void __user *)getaddrs.addrs;
	list_for_each(pos, &asoc->peer.transport_addr_list) {
		from = list_entry(pos, struct sctp_transport, transports);
		memcpy(&temp, &from->ipaddr, sizeof(temp));
		sctp_get_pf_specific(sk->sk_family)->addr_v4map(sp, &temp);
		addrlen = sctp_get_af_specific(sk->sk_family)->sockaddr_len;
		if (copy_to_user(to, &temp, addrlen))
			return -EFAULT;
		to += addrlen ;
		cnt ++;
		if (cnt >= getaddrs.addr_num) break;
	}
	getaddrs.addr_num = cnt;
	if (copy_to_user(optval, &getaddrs, sizeof(struct sctp_getaddrs_old)))
		return -EFAULT;

	return 0;
}
