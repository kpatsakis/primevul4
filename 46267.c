rds_send_pong(struct rds_connection *conn, __be16 dport)
{
	struct rds_message *rm;
	unsigned long flags;
	int ret = 0;

	rm = rds_message_alloc(0, GFP_ATOMIC);
	if (!rm) {
		ret = -ENOMEM;
		goto out;
	}

	rm->m_daddr = conn->c_faddr;
	rm->data.op_active = 1;

	rds_conn_connect_if_down(conn);

	ret = rds_cong_wait(conn->c_fcong, dport, 1, NULL);
	if (ret)
		goto out;

	spin_lock_irqsave(&conn->c_lock, flags);
	list_add_tail(&rm->m_conn_item, &conn->c_send_queue);
	set_bit(RDS_MSG_ON_CONN, &rm->m_flags);
	rds_message_addref(rm);
	rm->m_inc.i_conn = conn;

	rds_message_populate_header(&rm->m_inc.i_hdr, 0, dport,
				    conn->c_next_tx_seq);
	conn->c_next_tx_seq++;
	spin_unlock_irqrestore(&conn->c_lock, flags);

	rds_stats_inc(s_send_queued);
	rds_stats_inc(s_send_pong);

	/* schedule the send work on rds_wq */
	queue_delayed_work(rds_wq, &conn->c_send_w, 1);

	rds_message_put(rm);
	return 0;

out:
	if (rm)
		rds_message_put(rm);
	return ret;
}
