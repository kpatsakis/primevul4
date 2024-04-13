static void pfkey_terminate_dump(struct pfkey_sock *pfk)
{
	if (pfk->dump.dump) {
		if (pfk->dump.skb) {
			kfree_skb(pfk->dump.skb);
			pfk->dump.skb = NULL;
		}
		pfk->dump.done(pfk);
		pfk->dump.dump = NULL;
		pfk->dump.done = NULL;
	}
}
