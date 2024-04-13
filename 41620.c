static int sctp_copy_laddrs(struct sock *sk, __u16 port, void *to,
			    size_t space_left, int *bytes_copied)
{
	struct sctp_sockaddr_entry *addr;
	union sctp_addr temp;
	int cnt = 0;
	int addrlen;
	struct net *net = sock_net(sk);

	rcu_read_lock();
	list_for_each_entry_rcu(addr, &net->sctp.local_addr_list, list) {
		if (!addr->valid)
			continue;

		if ((PF_INET == sk->sk_family) &&
		    (AF_INET6 == addr->a.sa.sa_family))
			continue;
		if ((PF_INET6 == sk->sk_family) &&
		    inet_v6_ipv6only(sk) &&
		    (AF_INET == addr->a.sa.sa_family))
			continue;
		memcpy(&temp, &addr->a, sizeof(temp));
		if (!temp.v4.sin_port)
			temp.v4.sin_port = htons(port);

		sctp_get_pf_specific(sk->sk_family)->addr_v4map(sctp_sk(sk),
								&temp);
		addrlen = sctp_get_af_specific(temp.sa.sa_family)->sockaddr_len;
		if (space_left < addrlen) {
			cnt =  -ENOMEM;
			break;
		}
		memcpy(to, &temp, addrlen);

		to += addrlen;
		cnt ++;
		space_left -= addrlen;
		*bytes_copied += addrlen;
	}
	rcu_read_unlock();

	return cnt;
}
