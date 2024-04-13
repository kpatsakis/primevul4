void sched_destroy_group(struct task_group *tg)
{
	/* wait for possible concurrent references to cfs_rqs complete */
	call_rcu(&tg->rcu, free_sched_group_rcu);
}
