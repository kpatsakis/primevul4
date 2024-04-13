static struct cgroup_subsys_state *perf_cgroup_css_alloc(struct cgroup *cont)
{
	struct perf_cgroup *jc;

	jc = kzalloc(sizeof(*jc), GFP_KERNEL);
	if (!jc)
		return ERR_PTR(-ENOMEM);

	jc->info = alloc_percpu(struct perf_cgroup_info);
	if (!jc->info) {
		kfree(jc);
		return ERR_PTR(-ENOMEM);
	}

	return &jc->css;
}
