struct rds_message *rds_send_get_message(struct rds_connection *conn,
					 struct rm_rdma_op *op)
{
	struct rds_message *rm, *tmp, *found = NULL;
	unsigned long flags;

	spin_lock_irqsave(&conn->c_lock, flags);

	list_for_each_entry_safe(rm, tmp, &conn->c_retrans, m_conn_item) {
		if (&rm->rdma == op) {
			atomic_inc(&rm->m_refcount);
			found = rm;
			goto out;
		}
	}

	list_for_each_entry_safe(rm, tmp, &conn->c_send_queue, m_conn_item) {
		if (&rm->rdma == op) {
			atomic_inc(&rm->m_refcount);
			found = rm;
			break;
		}
	}

out:
	spin_unlock_irqrestore(&conn->c_lock, flags);

	return found;
}
