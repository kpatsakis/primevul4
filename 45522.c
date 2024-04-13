static void ipxitf_insert_socket(struct ipx_interface *intrfc, struct sock *sk)
{
	ipxitf_hold(intrfc);
	spin_lock_bh(&intrfc->if_sklist_lock);
	ipx_sk(sk)->intrfc = intrfc;
	sk_add_node(sk, &intrfc->if_sklist);
	spin_unlock_bh(&intrfc->if_sklist_lock);
	ipxitf_put(intrfc);
}
