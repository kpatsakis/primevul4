int ndisc_ifinfo_sysctl_change(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct net_device *dev = ctl->extra1;
	struct inet6_dev *idev;
	int ret;

	if ((strcmp(ctl->procname, "retrans_time") == 0) ||
	    (strcmp(ctl->procname, "base_reachable_time") == 0))
		ndisc_warn_deprecated_sysctl(ctl, "syscall", dev ? dev->name : "default");

	if (strcmp(ctl->procname, "retrans_time") == 0)
		ret = neigh_proc_dointvec(ctl, write, buffer, lenp, ppos);

	else if (strcmp(ctl->procname, "base_reachable_time") == 0)
		ret = neigh_proc_dointvec_jiffies(ctl, write,
						  buffer, lenp, ppos);

	else if ((strcmp(ctl->procname, "retrans_time_ms") == 0) ||
		 (strcmp(ctl->procname, "base_reachable_time_ms") == 0))
		ret = neigh_proc_dointvec_ms_jiffies(ctl, write,
						     buffer, lenp, ppos);
	else
		ret = -1;

	if (write && ret == 0 && dev && (idev = in6_dev_get(dev)) != NULL) {
		if (ctl->data == &NEIGH_VAR(idev->nd_parms, BASE_REACHABLE_TIME))
			idev->nd_parms->reachable_time =
					neigh_rand_reach_time(NEIGH_VAR(idev->nd_parms, BASE_REACHABLE_TIME));
		idev->tstamp = jiffies;
		inet6_ifinfo_notify(RTM_NEWLINK, idev);
		in6_dev_put(idev);
	}
	return ret;
}
