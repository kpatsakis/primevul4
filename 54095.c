__checkparam_dl(const struct sched_attr *attr)
{
	return attr && attr->sched_deadline != 0 &&
		(attr->sched_period == 0 ||
		(s64)(attr->sched_period   - attr->sched_deadline) >= 0) &&
		(s64)(attr->sched_deadline - attr->sched_runtime ) >= 0  &&
		attr->sched_runtime >= (2 << (DL_SCALE - 1));
}
