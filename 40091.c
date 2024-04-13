static void __vcc_insert_socket(struct sock *sk)
{
	struct atm_vcc *vcc = atm_sk(sk);
	struct hlist_head *head = &vcc_hash[vcc->vci & (VCC_HTABLE_SIZE - 1)];
	sk->sk_hash = vcc->vci & (VCC_HTABLE_SIZE - 1);
	sk_add_node(sk, head);
}
