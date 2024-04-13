void rds_send_drop_acked(struct rds_connection *conn, u64 ack,
			 is_acked_func is_acked)
{
	struct rds_message *rm, *tmp;
	unsigned long flags;
	LIST_HEAD(list);

	spin_lock_irqsave(&conn->c_lock, flags);

	list_for_each_entry_safe(rm, tmp, &conn->c_retrans, m_conn_item) {
		if (!rds_send_is_acked(rm, ack, is_acked))
			break;

		list_move(&rm->m_conn_item, &list);
		clear_bit(RDS_MSG_ON_CONN, &rm->m_flags);
	}

	/* order flag updates with spin locks */
	if (!list_empty(&list))
		smp_mb__after_atomic();

	spin_unlock_irqrestore(&conn->c_lock, flags);

	/* now remove the messages from the sock list as needed */
	rds_send_remove_from_sock(&list, RDS_RDMA_SUCCESS);
}
