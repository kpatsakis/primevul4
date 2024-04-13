static inline int is_root(void)
{
	return geteuid() == 0;
}