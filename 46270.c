void rds_send_reset(struct rds_connection *conn)
{
	struct rds_message *rm, *tmp;
	unsigned long flags;

	if (conn->c_xmit_rm) {
		rm = conn->c_xmit_rm;
		conn->c_xmit_rm = NULL;
		/* Tell the user the RDMA op is no longer mapped by the
		 * transport. This isn't entirely true (it's flushed out
		 * independently) but as the connection is down, there's
		 * no ongoing RDMA to/from that memory */
		rds_message_unmapped(rm);
		rds_message_put(rm);
	}

	conn->c_xmit_sg = 0;
	conn->c_xmit_hdr_off = 0;
	conn->c_xmit_data_off = 0;
	conn->c_xmit_atomic_sent = 0;
	conn->c_xmit_rdma_sent = 0;
	conn->c_xmit_data_sent = 0;

	conn->c_map_queued = 0;

	conn->c_unacked_packets = rds_sysctl_max_unacked_packets;
	conn->c_unacked_bytes = rds_sysctl_max_unacked_bytes;

	/* Mark messages as retransmissions, and move them to the send q */
	spin_lock_irqsave(&conn->c_lock, flags);
	list_for_each_entry_safe(rm, tmp, &conn->c_retrans, m_conn_item) {
		set_bit(RDS_MSG_ACK_REQUIRED, &rm->m_flags);
		set_bit(RDS_MSG_RETRANSMITTED, &rm->m_flags);
	}
	list_splice_init(&conn->c_retrans, &conn->c_send_queue);
	spin_unlock_irqrestore(&conn->c_lock, flags);
}
