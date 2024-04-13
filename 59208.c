static bool is_sb_event(struct perf_event *event)
{
	struct perf_event_attr *attr = &event->attr;

	if (event->parent)
		return false;

	if (event->attach_state & PERF_ATTACH_TASK)
		return false;

	if (attr->mmap || attr->mmap_data || attr->mmap2 ||
	    attr->comm || attr->comm_exec ||
	    attr->task ||
	    attr->context_switch)
		return true;
	return false;
}
