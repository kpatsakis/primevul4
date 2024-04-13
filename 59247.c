static int perf_event_period(struct perf_event *event, u64 __user *arg)
{
	u64 value;

	if (!is_sampling_event(event))
		return -EINVAL;

	if (copy_from_user(&value, arg, sizeof(value)))
		return -EFAULT;

	if (!value)
		return -EINVAL;

	if (event->attr.freq && value > sysctl_perf_event_sample_rate)
		return -EINVAL;

	event_function_call(event, __perf_event_period, &value);

	return 0;
}
