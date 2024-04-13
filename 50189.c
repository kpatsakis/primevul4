kuid_t sock_i_uid(struct sock *sk)
{
	kuid_t uid;

	read_lock_bh(&sk->sk_callback_lock);
	uid = sk->sk_socket ? SOCK_INODE(sk->sk_socket)->i_uid : GLOBAL_ROOT_UID;
	read_unlock_bh(&sk->sk_callback_lock);
	return uid;
}
