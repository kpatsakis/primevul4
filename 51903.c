static void ppp_destroy_interface(struct ppp *ppp)
{
	atomic_dec(&ppp_unit_count);

	if (!ppp->file.dead || ppp->n_channels) {
		/* "can't happen" */
		netdev_err(ppp->dev, "ppp: destroying ppp struct %p "
			   "but dead=%d n_channels=%d !\n",
			   ppp, ppp->file.dead, ppp->n_channels);
		return;
	}

	ppp_ccp_closed(ppp);
	if (ppp->vj) {
		slhc_free(ppp->vj);
		ppp->vj = NULL;
	}
	skb_queue_purge(&ppp->file.xq);
	skb_queue_purge(&ppp->file.rq);
#ifdef CONFIG_PPP_MULTILINK
	skb_queue_purge(&ppp->mrq);
#endif /* CONFIG_PPP_MULTILINK */
#ifdef CONFIG_PPP_FILTER
	if (ppp->pass_filter) {
		bpf_prog_destroy(ppp->pass_filter);
		ppp->pass_filter = NULL;
	}

	if (ppp->active_filter) {
		bpf_prog_destroy(ppp->active_filter);
		ppp->active_filter = NULL;
	}
#endif /* CONFIG_PPP_FILTER */

	kfree_skb(ppp->xmit_pending);

	free_netdev(ppp->dev);
}
