static int cpu_cgroup_css_online(struct cgroup_subsys_state *css)
{
	struct task_group *tg = css_tg(css);
	struct task_group *parent = css_tg(css_parent(css));

	if (parent)
		sched_online_group(tg, parent);
	return 0;
}
