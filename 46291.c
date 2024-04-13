static void freeque(struct ipc_namespace *ns, struct kern_ipc_perm *ipcp)
{
	struct msg_msg *msg, *t;
	struct msg_queue *msq = container_of(ipcp, struct msg_queue, q_perm);

	expunge_all(msq, -EIDRM);
	ss_wakeup(&msq->q_senders, 1);
	msg_rmid(ns, msq);
	ipc_unlock_object(&msq->q_perm);
	rcu_read_unlock();

	list_for_each_entry_safe(msg, t, &msq->q_messages, m_list) {
		atomic_dec(&ns->msg_hdrs);
		free_msg(msg);
	}
	atomic_sub(msq->q_cbytes, &ns->msg_bytes);
	ipc_rcu_putref(msq, msg_rcu_free);
}
