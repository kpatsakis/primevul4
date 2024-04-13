static int sctp_getsockopt_local_addrs(struct sock *sk, int len,
				       char __user *optval, int __user *optlen)
{
	struct sctp_bind_addr *bp;
	struct sctp_association *asoc;
	struct list_head *pos;
	int cnt = 0;
	struct sctp_getaddrs getaddrs;
	struct sctp_sockaddr_entry *addr;
	void __user *to;
	union sctp_addr temp;
	struct sctp_sock *sp = sctp_sk(sk);
	int addrlen;
	rwlock_t *addr_lock;
	int err = 0;
	size_t space_left;
	int bytes_copied;

	if (len <= sizeof(struct sctp_getaddrs))
		return -EINVAL;

	if (copy_from_user(&getaddrs, optval, sizeof(struct sctp_getaddrs)))
		return -EFAULT;

	/*
	 *  For UDP-style sockets, id specifies the association to query.
	 *  If the id field is set to the value '0' then the locally bound
	 *  addresses are returned without regard to any particular
	 *  association.
	 */
	if (0 == getaddrs.assoc_id) {
		bp = &sctp_sk(sk)->ep->base.bind_addr;
		addr_lock = &sctp_sk(sk)->ep->base.addr_lock;
	} else {
		asoc = sctp_id2assoc(sk, getaddrs.assoc_id);
		if (!asoc)
			return -EINVAL;
		bp = &asoc->base.bind_addr;
		addr_lock = &asoc->base.addr_lock;
	}

	to = optval + offsetof(struct sctp_getaddrs,addrs);
	space_left = len - sizeof(struct sctp_getaddrs) -
			 offsetof(struct sctp_getaddrs,addrs);

	sctp_read_lock(addr_lock);

	/* If the endpoint is bound to 0.0.0.0 or ::0, get the valid
	 * addresses from the global local address list.
	 */
	if (sctp_list_single_entry(&bp->address_list)) {
		addr = list_entry(bp->address_list.next,
				  struct sctp_sockaddr_entry, list);
		if (sctp_is_any(&addr->a)) {
			cnt = sctp_copy_laddrs_to_user(sk, bp->port,
						       &to, space_left);
			if (cnt < 0) {
				err = cnt;
				goto unlock;
			}
			goto copy_getaddrs;
		}
	}

	list_for_each(pos, &bp->address_list) {
		addr = list_entry(pos, struct sctp_sockaddr_entry, list);
		memcpy(&temp, &addr->a, sizeof(temp));
		sctp_get_pf_specific(sk->sk_family)->addr_v4map(sp, &temp);
		addrlen = sctp_get_af_specific(temp.sa.sa_family)->sockaddr_len;
		if(space_left < addrlen)
			return -ENOMEM; /*fixme: right error?*/
		if (copy_to_user(to, &temp, addrlen)) {
			err = -EFAULT;
			goto unlock;
		}
		to += addrlen;
		cnt ++;
		space_left -= addrlen;
	}

copy_getaddrs:
	if (put_user(cnt, &((struct sctp_getaddrs __user *)optval)->addr_num))
		return -EFAULT;
	bytes_copied = ((char __user *)to) - optval;
	if (put_user(bytes_copied, optlen))
		return -EFAULT;

unlock:
	sctp_read_unlock(addr_lock);
	return err;
}
