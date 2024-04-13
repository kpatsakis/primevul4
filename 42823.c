static int process_backlog(struct napi_struct *napi, int quota)
{
	int work = 0;
	struct softnet_data *sd = container_of(napi, struct softnet_data, backlog);

#ifdef CONFIG_RPS
	/* Check if we have pending ipi, its better to send them now,
	 * not waiting net_rx_action() end.
	 */
	if (sd->rps_ipi_list) {
		local_irq_disable();
		net_rps_action_and_irq_enable(sd);
	}
#endif
	napi->weight = weight_p;
	local_irq_disable();
	while (work < quota) {
		struct sk_buff *skb;
		unsigned int qlen;

		while ((skb = __skb_dequeue(&sd->process_queue))) {
			local_irq_enable();
			__netif_receive_skb(skb);
			local_irq_disable();
			input_queue_head_incr(sd);
			if (++work >= quota) {
				local_irq_enable();
				return work;
			}
		}

		rps_lock(sd);
		qlen = skb_queue_len(&sd->input_pkt_queue);
		if (qlen)
			skb_queue_splice_tail_init(&sd->input_pkt_queue,
						   &sd->process_queue);

		if (qlen < quota - work) {
			/*
			 * Inline a custom version of __napi_complete().
			 * only current cpu owns and manipulates this napi,
			 * and NAPI_STATE_SCHED is the only possible flag set on backlog.
			 * we can use a plain write instead of clear_bit(),
			 * and we dont need an smp_mb() memory barrier.
			 */
			list_del(&napi->poll_list);
			napi->state = 0;

			quota = work + qlen;
		}
		rps_unlock(sd);
	}
	local_irq_enable();

	return work;
}
