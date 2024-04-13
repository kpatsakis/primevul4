static inline int cma_any_port(struct sockaddr *addr)
{
	return !cma_port(addr);
}
