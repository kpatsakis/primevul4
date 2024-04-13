void bt_sock_reclassify_lock(struct sock *sk, int proto)
{
	BUG_ON(!sk);
	BUG_ON(sock_owned_by_user(sk));

	sock_lock_init_class_and_name(sk,
			bt_slock_key_strings[proto], &bt_slock_key[proto],
				bt_key_strings[proto], &bt_lock_key[proto]);
}
