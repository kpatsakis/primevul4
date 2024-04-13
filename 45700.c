static void unregister_prot_hook(struct sock *sk, bool sync)
{
	struct packet_sock *po = pkt_sk(sk);

	if (po->running)
		__unregister_prot_hook(sk, sync);
}
