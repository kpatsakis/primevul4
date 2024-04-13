int rds_send_xmit(struct rds_connection *conn)
{
	struct rds_message *rm;
	unsigned long flags;
	unsigned int tmp;
	struct scatterlist *sg;
	int ret = 0;
	LIST_HEAD(to_be_dropped);
	int batch_count;
	unsigned long send_gen = 0;

restart:
	batch_count = 0;

	/*
	 * sendmsg calls here after having queued its message on the send
	 * queue.  We only have one task feeding the connection at a time.  If
	 * another thread is already feeding the queue then we back off.  This
	 * avoids blocking the caller and trading per-connection data between
	 * caches per message.
	 */
	if (!acquire_in_xmit(conn)) {
		rds_stats_inc(s_send_lock_contention);
		ret = -ENOMEM;
		goto out;
	}

	/*
	 * we record the send generation after doing the xmit acquire.
	 * if someone else manages to jump in and do some work, we'll use
	 * this to avoid a goto restart farther down.
	 *
	 * The acquire_in_xmit() check above ensures that only one
	 * caller can increment c_send_gen at any time.
	 */
	conn->c_send_gen++;
	send_gen = conn->c_send_gen;

	/*
	 * rds_conn_shutdown() sets the conn state and then tests RDS_IN_XMIT,
	 * we do the opposite to avoid races.
	 */
	if (!rds_conn_up(conn)) {
		release_in_xmit(conn);
		ret = 0;
		goto out;
	}

	if (conn->c_trans->xmit_prepare)
		conn->c_trans->xmit_prepare(conn);

	/*
	 * spin trying to push headers and data down the connection until
	 * the connection doesn't make forward progress.
	 */
	while (1) {

		rm = conn->c_xmit_rm;

		/*
		 * If between sending messages, we can send a pending congestion
		 * map update.
		 */
		if (!rm && test_and_clear_bit(0, &conn->c_map_queued)) {
			rm = rds_cong_update_alloc(conn);
			if (IS_ERR(rm)) {
				ret = PTR_ERR(rm);
				break;
			}
			rm->data.op_active = 1;

			conn->c_xmit_rm = rm;
		}

		/*
		 * If not already working on one, grab the next message.
		 *
		 * c_xmit_rm holds a ref while we're sending this message down
		 * the connction.  We can use this ref while holding the
		 * send_sem.. rds_send_reset() is serialized with it.
		 */
		if (!rm) {
			unsigned int len;

			batch_count++;

			/* we want to process as big a batch as we can, but
			 * we also want to avoid softlockups.  If we've been
			 * through a lot of messages, lets back off and see
			 * if anyone else jumps in
			 */
			if (batch_count >= send_batch_count)
				goto over_batch;

			spin_lock_irqsave(&conn->c_lock, flags);

			if (!list_empty(&conn->c_send_queue)) {
				rm = list_entry(conn->c_send_queue.next,
						struct rds_message,
						m_conn_item);
				rds_message_addref(rm);

				/*
				 * Move the message from the send queue to the retransmit
				 * list right away.
				 */
				list_move_tail(&rm->m_conn_item, &conn->c_retrans);
			}

			spin_unlock_irqrestore(&conn->c_lock, flags);

			if (!rm)
				break;

			/* Unfortunately, the way Infiniband deals with
			 * RDMA to a bad MR key is by moving the entire
			 * queue pair to error state. We cold possibly
			 * recover from that, but right now we drop the
			 * connection.
			 * Therefore, we never retransmit messages with RDMA ops.
			 */
			if (rm->rdma.op_active &&
			    test_bit(RDS_MSG_RETRANSMITTED, &rm->m_flags)) {
				spin_lock_irqsave(&conn->c_lock, flags);
				if (test_and_clear_bit(RDS_MSG_ON_CONN, &rm->m_flags))
					list_move(&rm->m_conn_item, &to_be_dropped);
				spin_unlock_irqrestore(&conn->c_lock, flags);
				continue;
			}

			/* Require an ACK every once in a while */
			len = ntohl(rm->m_inc.i_hdr.h_len);
			if (conn->c_unacked_packets == 0 ||
			    conn->c_unacked_bytes < len) {
				__set_bit(RDS_MSG_ACK_REQUIRED, &rm->m_flags);

				conn->c_unacked_packets = rds_sysctl_max_unacked_packets;
				conn->c_unacked_bytes = rds_sysctl_max_unacked_bytes;
				rds_stats_inc(s_send_ack_required);
			} else {
				conn->c_unacked_bytes -= len;
				conn->c_unacked_packets--;
			}

			conn->c_xmit_rm = rm;
		}

		/* The transport either sends the whole rdma or none of it */
		if (rm->rdma.op_active && !conn->c_xmit_rdma_sent) {
			rm->m_final_op = &rm->rdma;
			/* The transport owns the mapped memory for now.
			 * You can't unmap it while it's on the send queue
			 */
			set_bit(RDS_MSG_MAPPED, &rm->m_flags);
			ret = conn->c_trans->xmit_rdma(conn, &rm->rdma);
			if (ret) {
				clear_bit(RDS_MSG_MAPPED, &rm->m_flags);
				wake_up_interruptible(&rm->m_flush_wait);
				break;
			}
			conn->c_xmit_rdma_sent = 1;

		}

		if (rm->atomic.op_active && !conn->c_xmit_atomic_sent) {
			rm->m_final_op = &rm->atomic;
			/* The transport owns the mapped memory for now.
			 * You can't unmap it while it's on the send queue
			 */
			set_bit(RDS_MSG_MAPPED, &rm->m_flags);
			ret = conn->c_trans->xmit_atomic(conn, &rm->atomic);
			if (ret) {
				clear_bit(RDS_MSG_MAPPED, &rm->m_flags);
				wake_up_interruptible(&rm->m_flush_wait);
				break;
			}
			conn->c_xmit_atomic_sent = 1;

		}

		/*
		 * A number of cases require an RDS header to be sent
		 * even if there is no data.
		 * We permit 0-byte sends; rds-ping depends on this.
		 * However, if there are exclusively attached silent ops,
		 * we skip the hdr/data send, to enable silent operation.
		 */
		if (rm->data.op_nents == 0) {
			int ops_present;
			int all_ops_are_silent = 1;

			ops_present = (rm->atomic.op_active || rm->rdma.op_active);
			if (rm->atomic.op_active && !rm->atomic.op_silent)
				all_ops_are_silent = 0;
			if (rm->rdma.op_active && !rm->rdma.op_silent)
				all_ops_are_silent = 0;

			if (ops_present && all_ops_are_silent
			    && !rm->m_rdma_cookie)
				rm->data.op_active = 0;
		}

		if (rm->data.op_active && !conn->c_xmit_data_sent) {
			rm->m_final_op = &rm->data;
			ret = conn->c_trans->xmit(conn, rm,
						  conn->c_xmit_hdr_off,
						  conn->c_xmit_sg,
						  conn->c_xmit_data_off);
			if (ret <= 0)
				break;

			if (conn->c_xmit_hdr_off < sizeof(struct rds_header)) {
				tmp = min_t(int, ret,
					    sizeof(struct rds_header) -
					    conn->c_xmit_hdr_off);
				conn->c_xmit_hdr_off += tmp;
				ret -= tmp;
			}

			sg = &rm->data.op_sg[conn->c_xmit_sg];
			while (ret) {
				tmp = min_t(int, ret, sg->length -
						      conn->c_xmit_data_off);
				conn->c_xmit_data_off += tmp;
				ret -= tmp;
				if (conn->c_xmit_data_off == sg->length) {
					conn->c_xmit_data_off = 0;
					sg++;
					conn->c_xmit_sg++;
					BUG_ON(ret != 0 &&
					       conn->c_xmit_sg == rm->data.op_nents);
				}
			}

			if (conn->c_xmit_hdr_off == sizeof(struct rds_header) &&
			    (conn->c_xmit_sg == rm->data.op_nents))
				conn->c_xmit_data_sent = 1;
		}

		/*
		 * A rm will only take multiple times through this loop
		 * if there is a data op. Thus, if the data is sent (or there was
		 * none), then we're done with the rm.
		 */
		if (!rm->data.op_active || conn->c_xmit_data_sent) {
			conn->c_xmit_rm = NULL;
			conn->c_xmit_sg = 0;
			conn->c_xmit_hdr_off = 0;
			conn->c_xmit_data_off = 0;
			conn->c_xmit_rdma_sent = 0;
			conn->c_xmit_atomic_sent = 0;
			conn->c_xmit_data_sent = 0;

			rds_message_put(rm);
		}
	}

over_batch:
	if (conn->c_trans->xmit_complete)
		conn->c_trans->xmit_complete(conn);
	release_in_xmit(conn);

	/* Nuke any messages we decided not to retransmit. */
	if (!list_empty(&to_be_dropped)) {
		/* irqs on here, so we can put(), unlike above */
		list_for_each_entry(rm, &to_be_dropped, m_conn_item)
			rds_message_put(rm);
		rds_send_remove_from_sock(&to_be_dropped, RDS_RDMA_DROPPED);
	}

	/*
	 * Other senders can queue a message after we last test the send queue
	 * but before we clear RDS_IN_XMIT.  In that case they'd back off and
	 * not try and send their newly queued message.  We need to check the
	 * send queue after having cleared RDS_IN_XMIT so that their message
	 * doesn't get stuck on the send queue.
	 *
	 * If the transport cannot continue (i.e ret != 0), then it must
	 * call us when more room is available, such as from the tx
	 * completion handler.
	 *
	 * We have an extra generation check here so that if someone manages
	 * to jump in after our release_in_xmit, we'll see that they have done
	 * some work and we will skip our goto
	 */
	if (ret == 0) {
		smp_mb();
		if ((test_bit(0, &conn->c_map_queued) ||
		     !list_empty(&conn->c_send_queue)) &&
		    send_gen == conn->c_send_gen) {
			rds_stats_inc(s_send_lock_queue_raced);
			if (batch_count < send_batch_count)
				goto restart;
			queue_delayed_work(rds_wq, &conn->c_send_w, 1);
		}
	}
out:
	return ret;
}
