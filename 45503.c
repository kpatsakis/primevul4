static void ipx_remove_socket(struct sock *sk)
{
	/* Determine interface with which socket is associated */
	struct ipx_interface *intrfc = ipx_sk(sk)->intrfc;

	if (!intrfc)
		goto out;

	ipxitf_hold(intrfc);
	spin_lock_bh(&intrfc->if_sklist_lock);
	sk_del_node_init(sk);
	spin_unlock_bh(&intrfc->if_sklist_lock);
	ipxitf_put(intrfc);
out:
	return;
}
