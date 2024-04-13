static void udp_v6_rehash(struct sock *sk)
{
	u16 new_hash = udp6_portaddr_hash(sock_net(sk),
					  &sk->sk_v6_rcv_saddr,
					  inet_sk(sk)->inet_num);

	udp_lib_rehash(sk, new_hash);
}
