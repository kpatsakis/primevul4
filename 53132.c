static void tcp_data_queue_ofo(struct sock *sk, struct sk_buff *skb)
{
	struct tcp_sock *tp = tcp_sk(sk);
	struct sk_buff *skb1;
	u32 seq, end_seq;

	tcp_ecn_check_ce(tp, skb);

	if (unlikely(tcp_try_rmem_schedule(sk, skb, skb->truesize))) {
		NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_TCPOFODROP);
		__kfree_skb(skb);
		return;
	}

	/* Disable header prediction. */
	tp->pred_flags = 0;
	inet_csk_schedule_ack(sk);

	NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_TCPOFOQUEUE);
	SOCK_DEBUG(sk, "out of order segment: rcv_next %X seq %X - %X\n",
		   tp->rcv_nxt, TCP_SKB_CB(skb)->seq, TCP_SKB_CB(skb)->end_seq);

	skb1 = skb_peek_tail(&tp->out_of_order_queue);
	if (!skb1) {
		/* Initial out of order segment, build 1 SACK. */
		if (tcp_is_sack(tp)) {
			tp->rx_opt.num_sacks = 1;
			tp->selective_acks[0].start_seq = TCP_SKB_CB(skb)->seq;
			tp->selective_acks[0].end_seq =
						TCP_SKB_CB(skb)->end_seq;
		}
		__skb_queue_head(&tp->out_of_order_queue, skb);
		goto end;
	}

	seq = TCP_SKB_CB(skb)->seq;
	end_seq = TCP_SKB_CB(skb)->end_seq;

	if (seq == TCP_SKB_CB(skb1)->end_seq) {
		bool fragstolen;

		if (!tcp_try_coalesce(sk, skb1, skb, &fragstolen)) {
			__skb_queue_after(&tp->out_of_order_queue, skb1, skb);
		} else {
			tcp_grow_window(sk, skb);
			kfree_skb_partial(skb, fragstolen);
			skb = NULL;
		}

		if (!tp->rx_opt.num_sacks ||
		    tp->selective_acks[0].end_seq != seq)
			goto add_sack;

		/* Common case: data arrive in order after hole. */
		tp->selective_acks[0].end_seq = end_seq;
		goto end;
	}

	/* Find place to insert this segment. */
	while (1) {
		if (!after(TCP_SKB_CB(skb1)->seq, seq))
			break;
		if (skb_queue_is_first(&tp->out_of_order_queue, skb1)) {
			skb1 = NULL;
			break;
		}
		skb1 = skb_queue_prev(&tp->out_of_order_queue, skb1);
	}

	/* Do skb overlap to previous one? */
	if (skb1 && before(seq, TCP_SKB_CB(skb1)->end_seq)) {
		if (!after(end_seq, TCP_SKB_CB(skb1)->end_seq)) {
			/* All the bits are present. Drop. */
			NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_TCPOFOMERGE);
			__kfree_skb(skb);
			skb = NULL;
			tcp_dsack_set(sk, seq, end_seq);
			goto add_sack;
		}
		if (after(seq, TCP_SKB_CB(skb1)->seq)) {
			/* Partial overlap. */
			tcp_dsack_set(sk, seq,
				      TCP_SKB_CB(skb1)->end_seq);
		} else {
			if (skb_queue_is_first(&tp->out_of_order_queue,
					       skb1))
				skb1 = NULL;
			else
				skb1 = skb_queue_prev(
					&tp->out_of_order_queue,
					skb1);
		}
	}
	if (!skb1)
		__skb_queue_head(&tp->out_of_order_queue, skb);
	else
		__skb_queue_after(&tp->out_of_order_queue, skb1, skb);

	/* And clean segments covered by new one as whole. */
	while (!skb_queue_is_last(&tp->out_of_order_queue, skb)) {
		skb1 = skb_queue_next(&tp->out_of_order_queue, skb);

		if (!after(end_seq, TCP_SKB_CB(skb1)->seq))
			break;
		if (before(end_seq, TCP_SKB_CB(skb1)->end_seq)) {
			tcp_dsack_extend(sk, TCP_SKB_CB(skb1)->seq,
					 end_seq);
			break;
		}
		__skb_unlink(skb1, &tp->out_of_order_queue);
		tcp_dsack_extend(sk, TCP_SKB_CB(skb1)->seq,
				 TCP_SKB_CB(skb1)->end_seq);
		NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_TCPOFOMERGE);
		__kfree_skb(skb1);
	}

add_sack:
	if (tcp_is_sack(tp))
		tcp_sack_new_ofo_skb(sk, seq, end_seq);
end:
	if (skb) {
		tcp_grow_window(sk, skb);
		skb_set_owner_r(skb, sk);
	}
}
