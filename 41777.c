static inline size_t xfrm_sadinfo_msgsize(void)
{
	return NLMSG_ALIGN(4)
	       + nla_total_size(sizeof(struct xfrmu_sadhinfo))
	       + nla_total_size(4); /* XFRMA_SAD_CNT */
}
