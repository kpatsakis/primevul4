struct task_group *sched_create_group(struct task_group *parent)
{
	struct task_group *tg;

	tg = kzalloc(sizeof(*tg), GFP_KERNEL);
	if (!tg)
		return ERR_PTR(-ENOMEM);

	if (!alloc_fair_sched_group(tg, parent))
		goto err;

	if (!alloc_rt_sched_group(tg, parent))
		goto err;

	return tg;

err:
	free_sched_group(tg);
	return ERR_PTR(-ENOMEM);
}
