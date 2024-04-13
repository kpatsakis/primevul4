static void ppp_destroy_channel(struct channel *pch)
{
	atomic_dec(&channel_count);

	if (!pch->file.dead) {
		/* "can't happen" */
		pr_err("ppp: destroying undead channel %p !\n", pch);
		return;
	}
	skb_queue_purge(&pch->file.xq);
	skb_queue_purge(&pch->file.rq);
	kfree(pch);
}
