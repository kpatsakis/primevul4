static void sched_ttwu_pending(void)
{
	struct rq *rq = this_rq();
	struct llist_node *llist = llist_del_all(&rq->wake_list);
	struct task_struct *p;

	raw_spin_lock(&rq->lock);

	while (llist) {
		p = llist_entry(llist, struct task_struct, wake_entry);
		llist = llist_next(llist);
		ttwu_do_activate(rq, p, 0);
	}

	raw_spin_unlock(&rq->lock);
}
