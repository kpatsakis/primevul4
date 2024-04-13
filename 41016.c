static inline bool perf_tryget_cgroup(struct perf_event *event)
{
	return css_tryget(&event->cgrp->css);
}
