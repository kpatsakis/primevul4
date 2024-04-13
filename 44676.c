static inline void debugt(const char *func, const char *msg)
{
	if (DP->flags & DEBUGT)
		pr_info("%s:%s dtime=%lu\n", func, msg, jiffies - debugtimer);
}
