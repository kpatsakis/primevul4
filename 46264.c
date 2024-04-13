void rds_send_drop_to(struct rds_sock *rs, struct sockaddr_in *dest)
{
	struct rds_message *rm, *tmp;
	struct rds_connection *conn;
	unsigned long flags;
	LIST_HEAD(list);

	/* get all the messages we're dropping under the rs lock */
	spin_lock_irqsave(&rs->rs_lock, flags);

	list_for_each_entry_safe(rm, tmp, &rs->rs_send_queue, m_sock_item) {
		if (dest && (dest->sin_addr.s_addr != rm->m_daddr ||
			     dest->sin_port != rm->m_inc.i_hdr.h_dport))
			continue;

		list_move(&rm->m_sock_item, &list);
		rds_send_sndbuf_remove(rs, rm);
		clear_bit(RDS_MSG_ON_SOCK, &rm->m_flags);
	}

	/* order flag updates with the rs lock */
	smp_mb__after_atomic();

	spin_unlock_irqrestore(&rs->rs_lock, flags);

	if (list_empty(&list))
		return;

	/* Remove the messages from the conn */
	list_for_each_entry(rm, &list, m_sock_item) {

		conn = rm->m_inc.i_conn;

		spin_lock_irqsave(&conn->c_lock, flags);
		/*
		 * Maybe someone else beat us to removing rm from the conn.
		 * If we race with their flag update we'll get the lock and
		 * then really see that the flag has been cleared.
		 */
		if (!test_and_clear_bit(RDS_MSG_ON_CONN, &rm->m_flags)) {
			spin_unlock_irqrestore(&conn->c_lock, flags);
			spin_lock_irqsave(&rm->m_rs_lock, flags);
			rm->m_rs = NULL;
			spin_unlock_irqrestore(&rm->m_rs_lock, flags);
			continue;
		}
		list_del_init(&rm->m_conn_item);
		spin_unlock_irqrestore(&conn->c_lock, flags);

		/*
		 * Couldn't grab m_rs_lock in top loop (lock ordering),
		 * but we can now.
		 */
		spin_lock_irqsave(&rm->m_rs_lock, flags);

		spin_lock(&rs->rs_lock);
		__rds_send_complete(rs, rm, RDS_RDMA_CANCELED);
		spin_unlock(&rs->rs_lock);

		rm->m_rs = NULL;
		spin_unlock_irqrestore(&rm->m_rs_lock, flags);

		rds_message_put(rm);
	}

	rds_wake_sk_sleep(rs);

	while (!list_empty(&list)) {
		rm = list_entry(list.next, struct rds_message, m_sock_item);
		list_del_init(&rm->m_sock_item);
		rds_message_wait(rm);

		/* just in case the code above skipped this message
		 * because RDS_MSG_ON_CONN wasn't set, run it again here
		 * taking m_rs_lock is the only thing that keeps us
		 * from racing with ack processing.
		 */
		spin_lock_irqsave(&rm->m_rs_lock, flags);

		spin_lock(&rs->rs_lock);
		__rds_send_complete(rs, rm, RDS_RDMA_CANCELED);
		spin_unlock(&rs->rs_lock);

		rm->m_rs = NULL;
		spin_unlock_irqrestore(&rm->m_rs_lock, flags);

		rds_message_put(rm);
	}
}
