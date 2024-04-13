static void rds_conn_message_info(struct socket *sock, unsigned int len,
				  struct rds_info_iterator *iter,
				  struct rds_info_lengths *lens,
				  int want_send)
{
	struct hlist_head *head;
	struct list_head *list;
	struct rds_connection *conn;
	struct rds_message *rm;
	unsigned int total = 0;
	unsigned long flags;
	size_t i;

	len /= sizeof(struct rds_info_message);

	rcu_read_lock();

	for (i = 0, head = rds_conn_hash; i < ARRAY_SIZE(rds_conn_hash);
	     i++, head++) {
		hlist_for_each_entry_rcu(conn, head, c_hash_node) {
			if (want_send)
				list = &conn->c_send_queue;
			else
				list = &conn->c_retrans;

			spin_lock_irqsave(&conn->c_lock, flags);

			/* XXX too lazy to maintain counts.. */
			list_for_each_entry(rm, list, m_conn_item) {
				total++;
				if (total <= len)
					rds_inc_info_copy(&rm->m_inc, iter,
							  conn->c_laddr,
							  conn->c_faddr, 0);
			}

			spin_unlock_irqrestore(&conn->c_lock, flags);
		}
	}
	rcu_read_unlock();

	lens->nr = total;
	lens->each = sizeof(struct rds_info_message);
}
