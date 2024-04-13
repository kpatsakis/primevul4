static int sctp_copy_laddrs_to_user_old(struct sock *sk, __u16 port, int max_addrs,
					void __user *to)
{
	struct list_head *pos, *next;
	struct sctp_sockaddr_entry *addr;
	union sctp_addr temp;
	int cnt = 0;
	int addrlen;

	list_for_each_safe(pos, next, &sctp_local_addr_list) {
		addr = list_entry(pos, struct sctp_sockaddr_entry, list);
		if ((PF_INET == sk->sk_family) &&
		    (AF_INET6 == addr->a.sa.sa_family))
			continue;
		memcpy(&temp, &addr->a, sizeof(temp));
		sctp_get_pf_specific(sk->sk_family)->addr_v4map(sctp_sk(sk),
								&temp);
		addrlen = sctp_get_af_specific(temp.sa.sa_family)->sockaddr_len;
		if (copy_to_user(to, &temp, addrlen))
			return -EFAULT;

		to += addrlen;
		cnt ++;
		if (cnt >= max_addrs) break;
	}

	return cnt;
}
