void vsock_for_each_connected_socket(void (*fn)(struct sock *sk))
{
	int i;

	spin_lock_bh(&vsock_table_lock);

	for (i = 0; i < ARRAY_SIZE(vsock_connected_table); i++) {
		struct vsock_sock *vsk;
		list_for_each_entry(vsk, &vsock_connected_table[i],
				    connected_table)
			fn(sk_vsock(vsk));
	}

	spin_unlock_bh(&vsock_table_lock);
}
