static void pppoe_flush_dev(struct net_device *dev)
{
	struct pppoe_net *pn;
	int i;

	pn = pppoe_pernet(dev_net(dev));
	write_lock_bh(&pn->hash_lock);
	for (i = 0; i < PPPOE_HASH_SIZE; i++) {
		struct pppox_sock *po = pn->hash_table[i];
		struct sock *sk;

		while (po) {
			while (po && po->pppoe_dev != dev) {
				po = po->next;
			}

			if (!po)
				break;

			sk = sk_pppox(po);

			/* We always grab the socket lock, followed by the
			 * hash_lock, in that order.  Since we should hold the
			 * sock lock while doing any unbinding, we need to
			 * release the lock we're holding.  Hold a reference to
			 * the sock so it doesn't disappear as we're jumping
			 * between locks.
			 */

			sock_hold(sk);
			write_unlock_bh(&pn->hash_lock);
			lock_sock(sk);

			if (po->pppoe_dev == dev &&
			    sk->sk_state & (PPPOX_CONNECTED | PPPOX_BOUND | PPPOX_ZOMBIE)) {
				pppox_unbind_sock(sk);
				sk->sk_state = PPPOX_ZOMBIE;
				sk->sk_state_change(sk);
				po->pppoe_dev = NULL;
				dev_put(dev);
			}

			release_sock(sk);
			sock_put(sk);

			/* Restart the process from the start of the current
			 * hash chain. We dropped locks so the world may have
			 * change from underneath us.
			 */

			BUG_ON(pppoe_pernet(dev_net(dev)) == NULL);
			write_lock_bh(&pn->hash_lock);
			po = pn->hash_table[i];
		}
	}
	write_unlock_bh(&pn->hash_lock);
}
