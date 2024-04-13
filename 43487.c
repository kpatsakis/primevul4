void cgtimer_time(cgtimer_t *ts_start)
{
	clock_serv_t cclock;
	mach_timespec_t mts;

	host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts_start->tv_sec = mts.tv_sec;
	ts_start->tv_nsec = mts.tv_nsec;
}
