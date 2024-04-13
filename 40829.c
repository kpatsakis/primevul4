static inline int is_si_special(const struct siginfo *info)
{
	return info <= SEND_SIG_FORCED;
}
