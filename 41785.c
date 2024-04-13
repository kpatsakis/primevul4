static inline size_t xfrm_spdinfo_msgsize(void)
{
	return NLMSG_ALIGN(4)
	       + nla_total_size(sizeof(struct xfrmu_spdinfo))
	       + nla_total_size(sizeof(struct xfrmu_spdhinfo));
}
