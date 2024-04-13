void x25_destroy_socket_from_timer(struct sock *sk)
{
	sock_hold(sk);
	bh_lock_sock(sk);
	__x25_destroy_socket(sk);
	bh_unlock_sock(sk);
	sock_put(sk);
}
