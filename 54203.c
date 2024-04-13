static inline int tg_has_rt_tasks(struct task_group *tg)
{
	struct task_struct *g, *p;

	do_each_thread(g, p) {
		if (rt_task(p) && task_rq(p)->rt.tg == tg)
			return 1;
	} while_each_thread(g, p);

	return 0;
}
