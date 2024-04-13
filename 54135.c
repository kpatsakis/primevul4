static void free_sched_groups(struct sched_group *sg, int free_sgp)
{
	struct sched_group *tmp, *first;

	if (!sg)
		return;

	first = sg;
	do {
		tmp = sg->next;

		if (free_sgp && atomic_dec_and_test(&sg->sgp->ref))
			kfree(sg->sgp);

		kfree(sg);
		sg = tmp;
	} while (sg != first);
}
