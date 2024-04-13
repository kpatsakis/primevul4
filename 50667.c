static bool match_fanout_group(struct packet_type *ptype, struct sock *sk)
{
	if (sk->sk_family != PF_PACKET)
		return false;

	return ptype->af_packet_priv == pkt_sk(sk)->fanout;
}
