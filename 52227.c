get_time_t_max(void)
{
#if defined(TIME_T_MAX)
	return TIME_T_MAX;
#else
	/* ISO C allows time_t to be a floating-point type,
	   but POSIX requires an integer type.  The following
	   should work on any system that follows the POSIX
	   conventions. */
	if (((time_t)0) < ((time_t)-1)) {
		/* Time_t is unsigned */
		return (~(time_t)0);
	} else {
		/* Time_t is signed. */
		/* Assume it's the same as int64_t or int32_t */
		if (sizeof(time_t) == sizeof(int64_t)) {
			return (time_t)INT64_MAX;
		} else {
			return (time_t)INT32_MAX;
		}
	}
#endif
}
