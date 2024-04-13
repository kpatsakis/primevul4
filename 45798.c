static void x25_insert_socket(struct sock *sk)
{
	write_lock_bh(&x25_list_lock);
	sk_add_node(sk, &x25_list);
	write_unlock_bh(&x25_list_lock);
}
