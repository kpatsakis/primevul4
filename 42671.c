static int sctp_getsockopt_peer_addrs_num_old(struct sock *sk, int len,
					      char __user *optval,
					      int __user *optlen)
{
	sctp_assoc_t id;
	struct sctp_association *asoc;
	struct list_head *pos;
	int cnt = 0;

	if (len != sizeof(sctp_assoc_t))
		return -EINVAL;

	if (copy_from_user(&id, optval, sizeof(sctp_assoc_t)))
		return -EFAULT;

	/* For UDP-style sockets, id specifies the association to query.  */
	asoc = sctp_id2assoc(sk, id);
	if (!asoc)
		return -EINVAL;

	list_for_each(pos, &asoc->peer.transport_addr_list) {
		cnt ++;
	}

	return cnt;
}
