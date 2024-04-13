static void vcc_remove_socket(struct sock *sk)
{
	write_lock_irq(&vcc_sklist_lock);
	sk_del_node_init(sk);
	write_unlock_irq(&vcc_sklist_lock);
}
