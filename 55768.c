int tcp_recvmsg(struct sock *sk, struct msghdr *msg, size_t len, int nonblock,
		int flags, int *addr_len)
{
	struct tcp_sock *tp = tcp_sk(sk);
	int copied = 0;
	u32 peek_seq;
	u32 *seq;
	unsigned long used;
	int err;
	int target;		/* Read at least this many bytes */
	long timeo;
	struct task_struct *user_recv = NULL;
	struct sk_buff *skb, *last;
	u32 urg_hole = 0;

	if (unlikely(flags & MSG_ERRQUEUE))
		return inet_recv_error(sk, msg, len, addr_len);

	if (sk_can_busy_loop(sk) && skb_queue_empty(&sk->sk_receive_queue) &&
	    (sk->sk_state == TCP_ESTABLISHED))
		sk_busy_loop(sk, nonblock);

	lock_sock(sk);

	err = -ENOTCONN;
	if (sk->sk_state == TCP_LISTEN)
		goto out;

	timeo = sock_rcvtimeo(sk, nonblock);

	/* Urgent data needs to be handled specially. */
	if (flags & MSG_OOB)
		goto recv_urg;

	if (unlikely(tp->repair)) {
		err = -EPERM;
		if (!(flags & MSG_PEEK))
			goto out;

		if (tp->repair_queue == TCP_SEND_QUEUE)
			goto recv_sndq;

		err = -EINVAL;
		if (tp->repair_queue == TCP_NO_QUEUE)
			goto out;

		/* 'common' recv queue MSG_PEEK-ing */
	}

	seq = &tp->copied_seq;
	if (flags & MSG_PEEK) {
		peek_seq = tp->copied_seq;
		seq = &peek_seq;
	}

	target = sock_rcvlowat(sk, flags & MSG_WAITALL, len);

	do {
		u32 offset;

		/* Are we at urgent data? Stop if we have read anything or have SIGURG pending. */
		if (tp->urg_data && tp->urg_seq == *seq) {
			if (copied)
				break;
			if (signal_pending(current)) {
				copied = timeo ? sock_intr_errno(timeo) : -EAGAIN;
				break;
			}
		}

		/* Next get a buffer. */

		last = skb_peek_tail(&sk->sk_receive_queue);
		skb_queue_walk(&sk->sk_receive_queue, skb) {
			last = skb;
			/* Now that we have two receive queues this
			 * shouldn't happen.
			 */
			if (WARN(before(*seq, TCP_SKB_CB(skb)->seq),
				 "recvmsg bug: copied %X seq %X rcvnxt %X fl %X\n",
				 *seq, TCP_SKB_CB(skb)->seq, tp->rcv_nxt,
				 flags))
				break;

			offset = *seq - TCP_SKB_CB(skb)->seq;
			if (unlikely(TCP_SKB_CB(skb)->tcp_flags & TCPHDR_SYN)) {
				pr_err_once("%s: found a SYN, please report !\n", __func__);
				offset--;
			}
			if (offset < skb->len)
				goto found_ok_skb;
			if (TCP_SKB_CB(skb)->tcp_flags & TCPHDR_FIN)
				goto found_fin_ok;
			WARN(!(flags & MSG_PEEK),
			     "recvmsg bug 2: copied %X seq %X rcvnxt %X fl %X\n",
			     *seq, TCP_SKB_CB(skb)->seq, tp->rcv_nxt, flags);
		}

		/* Well, if we have backlog, try to process it now yet. */

		if (copied >= target && !sk->sk_backlog.tail)
			break;

		if (copied) {
			if (sk->sk_err ||
			    sk->sk_state == TCP_CLOSE ||
			    (sk->sk_shutdown & RCV_SHUTDOWN) ||
			    !timeo ||
			    signal_pending(current))
				break;
		} else {
			if (sock_flag(sk, SOCK_DONE))
				break;

			if (sk->sk_err) {
				copied = sock_error(sk);
				break;
			}

			if (sk->sk_shutdown & RCV_SHUTDOWN)
				break;

			if (sk->sk_state == TCP_CLOSE) {
				if (!sock_flag(sk, SOCK_DONE)) {
					/* This occurs when user tries to read
					 * from never connected socket.
					 */
					copied = -ENOTCONN;
					break;
				}
				break;
			}

			if (!timeo) {
				copied = -EAGAIN;
				break;
			}

			if (signal_pending(current)) {
				copied = sock_intr_errno(timeo);
				break;
			}
		}

		tcp_cleanup_rbuf(sk, copied);

		if (!sysctl_tcp_low_latency && tp->ucopy.task == user_recv) {
			/* Install new reader */
			if (!user_recv && !(flags & (MSG_TRUNC | MSG_PEEK))) {
				user_recv = current;
				tp->ucopy.task = user_recv;
				tp->ucopy.msg = msg;
			}

			tp->ucopy.len = len;

			WARN_ON(tp->copied_seq != tp->rcv_nxt &&
				!(flags & (MSG_PEEK | MSG_TRUNC)));

			/* Ugly... If prequeue is not empty, we have to
			 * process it before releasing socket, otherwise
			 * order will be broken at second iteration.
			 * More elegant solution is required!!!
			 *
			 * Look: we have the following (pseudo)queues:
			 *
			 * 1. packets in flight
			 * 2. backlog
			 * 3. prequeue
			 * 4. receive_queue
			 *
			 * Each queue can be processed only if the next ones
			 * are empty. At this point we have empty receive_queue.
			 * But prequeue _can_ be not empty after 2nd iteration,
			 * when we jumped to start of loop because backlog
			 * processing added something to receive_queue.
			 * We cannot release_sock(), because backlog contains
			 * packets arrived _after_ prequeued ones.
			 *
			 * Shortly, algorithm is clear --- to process all
			 * the queues in order. We could make it more directly,
			 * requeueing packets from backlog to prequeue, if
			 * is not empty. It is more elegant, but eats cycles,
			 * unfortunately.
			 */
			if (!skb_queue_empty(&tp->ucopy.prequeue))
				goto do_prequeue;

			/* __ Set realtime policy in scheduler __ */
		}

		if (copied >= target) {
			/* Do not sleep, just process backlog. */
			release_sock(sk);
			lock_sock(sk);
		} else {
			sk_wait_data(sk, &timeo, last);
		}

		if (user_recv) {
			int chunk;

			/* __ Restore normal policy in scheduler __ */

			chunk = len - tp->ucopy.len;
			if (chunk != 0) {
				NET_ADD_STATS(sock_net(sk), LINUX_MIB_TCPDIRECTCOPYFROMBACKLOG, chunk);
				len -= chunk;
				copied += chunk;
			}

			if (tp->rcv_nxt == tp->copied_seq &&
			    !skb_queue_empty(&tp->ucopy.prequeue)) {
do_prequeue:
				tcp_prequeue_process(sk);

				chunk = len - tp->ucopy.len;
				if (chunk != 0) {
					NET_ADD_STATS(sock_net(sk), LINUX_MIB_TCPDIRECTCOPYFROMPREQUEUE, chunk);
					len -= chunk;
					copied += chunk;
				}
			}
		}
		if ((flags & MSG_PEEK) &&
		    (peek_seq - copied - urg_hole != tp->copied_seq)) {
			net_dbg_ratelimited("TCP(%s:%d): Application bug, race in MSG_PEEK\n",
					    current->comm,
					    task_pid_nr(current));
			peek_seq = tp->copied_seq;
		}
		continue;

	found_ok_skb:
		/* Ok so how much can we use? */
		used = skb->len - offset;
		if (len < used)
			used = len;

		/* Do we have urgent data here? */
		if (tp->urg_data) {
			u32 urg_offset = tp->urg_seq - *seq;
			if (urg_offset < used) {
				if (!urg_offset) {
					if (!sock_flag(sk, SOCK_URGINLINE)) {
						++*seq;
						urg_hole++;
						offset++;
						used--;
						if (!used)
							goto skip_copy;
					}
				} else
					used = urg_offset;
			}
		}

		if (!(flags & MSG_TRUNC)) {
			err = skb_copy_datagram_msg(skb, offset, msg, used);
			if (err) {
				/* Exception. Bailout! */
				if (!copied)
					copied = -EFAULT;
				break;
			}
		}

		*seq += used;
		copied += used;
		len -= used;

		tcp_rcv_space_adjust(sk);

skip_copy:
		if (tp->urg_data && after(tp->copied_seq, tp->urg_seq)) {
			tp->urg_data = 0;
			tcp_fast_path_check(sk);
		}
		if (used + offset < skb->len)
			continue;

		if (TCP_SKB_CB(skb)->tcp_flags & TCPHDR_FIN)
			goto found_fin_ok;
		if (!(flags & MSG_PEEK))
			sk_eat_skb(sk, skb);
		continue;

	found_fin_ok:
		/* Process the FIN. */
		++*seq;
		if (!(flags & MSG_PEEK))
			sk_eat_skb(sk, skb);
		break;
	} while (len > 0);

	if (user_recv) {
		if (!skb_queue_empty(&tp->ucopy.prequeue)) {
			int chunk;

			tp->ucopy.len = copied > 0 ? len : 0;

			tcp_prequeue_process(sk);

			if (copied > 0 && (chunk = len - tp->ucopy.len) != 0) {
				NET_ADD_STATS(sock_net(sk), LINUX_MIB_TCPDIRECTCOPYFROMPREQUEUE, chunk);
				len -= chunk;
				copied += chunk;
			}
		}

		tp->ucopy.task = NULL;
		tp->ucopy.len = 0;
	}

	/* According to UNIX98, msg_name/msg_namelen are ignored
	 * on connected socket. I was just happy when found this 8) --ANK
	 */

	/* Clean up data we have read: This will do ACK frames. */
	tcp_cleanup_rbuf(sk, copied);

	release_sock(sk);
	return copied;

out:
	release_sock(sk);
	return err;

recv_urg:
	err = tcp_recv_urg(sk, msg, len, flags);
	goto out;

recv_sndq:
	err = tcp_peek_sndq(sk, msg, len);
	goto out;
}
