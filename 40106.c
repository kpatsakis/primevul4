void vcc_insert_socket(struct sock *sk)
{
	write_lock_irq(&vcc_sklist_lock);
	__vcc_insert_socket(sk);
	write_unlock_irq(&vcc_sklist_lock);
}
