static inline int cma_any_addr(struct sockaddr *addr)
{
	return cma_zero_addr(addr) || cma_loopback_addr(addr);
}
