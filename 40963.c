static inline int perf_cgroup_connect(int fd, struct perf_event *event,
				      struct perf_event_attr *attr,
				      struct perf_event *group_leader)
{
	struct perf_cgroup *cgrp;
	struct cgroup_subsys_state *css;
	struct fd f = fdget(fd);
	int ret = 0;

	if (!f.file)
		return -EBADF;

	css = cgroup_css_from_dir(f.file, perf_subsys_id);
	if (IS_ERR(css)) {
		ret = PTR_ERR(css);
		goto out;
	}

	cgrp = container_of(css, struct perf_cgroup, css);
	event->cgrp = cgrp;

	/* must be done before we fput() the file */
	if (!perf_tryget_cgroup(event)) {
		event->cgrp = NULL;
		ret = -ENOENT;
		goto out;
	}

	/*
	 * all events in a group must monitor
	 * the same cgroup because a task belongs
	 * to only one perf cgroup at a time
	 */
	if (group_leader && group_leader->cgrp != cgrp) {
		perf_detach_cgroup(event);
		ret = -EINVAL;
	}
out:
	fdput(f);
	return ret;
}
