static void vcc_destroy_socket(struct sock *sk)
{
	struct atm_vcc *vcc = atm_sk(sk);
	struct sk_buff *skb;

	set_bit(ATM_VF_CLOSE, &vcc->flags);
	clear_bit(ATM_VF_READY, &vcc->flags);
	if (vcc->dev) {
		if (vcc->dev->ops->close)
			vcc->dev->ops->close(vcc);
		if (vcc->push)
			vcc->push(vcc, NULL); /* atmarpd has no push */

		while ((skb = skb_dequeue(&sk->sk_receive_queue)) != NULL) {
			atm_return(vcc, skb->truesize);
			kfree_skb(skb);
		}

		module_put(vcc->dev->ops->owner);
		atm_dev_put(vcc->dev);
	}

	vcc_remove_socket(sk);
}
