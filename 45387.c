static inline void __atalk_insert_socket(struct sock *sk)
{
	sk_add_node(sk, &atalk_sockets);
}
