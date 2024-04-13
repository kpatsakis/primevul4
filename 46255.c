void rds_conn_shutdown(struct rds_connection *conn)
{
	/* shut it down unless it's down already */
	if (!rds_conn_transition(conn, RDS_CONN_DOWN, RDS_CONN_DOWN)) {
		/*
		 * Quiesce the connection mgmt handlers before we start tearing
		 * things down. We don't hold the mutex for the entire
		 * duration of the shutdown operation, else we may be
		 * deadlocking with the CM handler. Instead, the CM event
		 * handler is supposed to check for state DISCONNECTING
		 */
		mutex_lock(&conn->c_cm_lock);
		if (!rds_conn_transition(conn, RDS_CONN_UP, RDS_CONN_DISCONNECTING)
		 && !rds_conn_transition(conn, RDS_CONN_ERROR, RDS_CONN_DISCONNECTING)) {
			rds_conn_error(conn, "shutdown called in state %d\n",
					atomic_read(&conn->c_state));
			mutex_unlock(&conn->c_cm_lock);
			return;
		}
		mutex_unlock(&conn->c_cm_lock);

		wait_event(conn->c_waitq,
			   !test_bit(RDS_IN_XMIT, &conn->c_flags));
		wait_event(conn->c_waitq,
			   !test_bit(RDS_RECV_REFILL, &conn->c_flags));

		conn->c_trans->conn_shutdown(conn);
		rds_conn_reset(conn);

		if (!rds_conn_transition(conn, RDS_CONN_DISCONNECTING, RDS_CONN_DOWN)) {
			/* This can happen - eg when we're in the middle of tearing
			 * down the connection, and someone unloads the rds module.
			 * Quite reproduceable with loopback connections.
			 * Mostly harmless.
			 */
			rds_conn_error(conn,
				"%s: failed to transition to state DOWN, "
				"current state is %d\n",
				__func__,
				atomic_read(&conn->c_state));
			return;
		}
	}

	/* Then reconnect if it's still live.
	 * The passive side of an IB loopback connection is never added
	 * to the conn hash, so we never trigger a reconnect on this
	 * conn - the reconnect is always triggered by the active peer. */
	cancel_delayed_work_sync(&conn->c_conn_w);
	rcu_read_lock();
	if (!hlist_unhashed(&conn->c_hash_node)) {
		rcu_read_unlock();
		if (conn->c_trans->t_type != RDS_TRANS_TCP ||
		    conn->c_outgoing == 1)
			rds_queue_reconnect(conn);
	} else {
		rcu_read_unlock();
	}
}
