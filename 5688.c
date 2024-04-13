static void set_scheduler(void)
{
	struct sched_param sched_param;
	struct rlimit rlimit;
	int rv;

	rlimit.rlim_cur = RLIM_INFINITY;
	rlimit.rlim_max = RLIM_INFINITY;
	rv = setrlimit(RLIMIT_MEMLOCK, &rlimit);
	if (rv < 0) {
		log_error("setrlimit failed");
	} else {
                rv = mlockall(MCL_CURRENT | MCL_FUTURE);
                if (rv < 0) {
                        log_error("mlockall failed");
                }
        }

	rv = sched_get_priority_max(SCHED_RR);
	if (rv != -1) {
		sched_param.sched_priority = rv;
		rv = sched_setscheduler(0, SCHED_RR, &sched_param);
		if (rv == -1)
			log_error("could not set SCHED_RR priority %d: %s (%d)",
					sched_param.sched_priority,
					strerror(errno), errno);
	} else {
		log_error("could not get maximum scheduler priority err %d",
				errno);
	}
}