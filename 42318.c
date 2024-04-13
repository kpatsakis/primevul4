void atm_dev_release_vccs(struct atm_dev *dev)
{
	int i;

	write_lock_irq(&vcc_sklist_lock);
	for (i = 0; i < VCC_HTABLE_SIZE; i++) {
		struct hlist_head *head = &vcc_hash[i];
		struct hlist_node *node, *tmp;
		struct sock *s;
		struct atm_vcc *vcc;

		sk_for_each_safe(s, node, tmp, head) {
			vcc = atm_sk(s);
			if (vcc->dev == dev) {
				vcc_release_async(vcc, -EPIPE);
				sk_del_node_init(s);
			}
		}
	}
	write_unlock_irq(&vcc_sklist_lock);
}
