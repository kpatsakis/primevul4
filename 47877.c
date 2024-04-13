static unsigned long get_reaper_busy(pid_t task)
{
	pid_t init = get_task_reaper_pid(task);
	char *cgroup = NULL, *usage_str = NULL;
	unsigned long usage = 0;

	if (init == -1)
		return 0;

	cgroup = get_pid_cgroup(task, "cpuacct");
	if (!cgroup)
		goto out;
	if (!cgfs_get_value("cpuacct", cgroup, "cpuacct.usage", &usage_str))
		goto out;
	usage = strtoul(usage_str, NULL, 10);
	usage /= 100000000;

out:
	free(cgroup);
	free(usage_str);
	return usage;
}
