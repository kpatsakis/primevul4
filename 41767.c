static inline size_t xfrm_mapping_msgsize(void)
{
	return NLMSG_ALIGN(sizeof(struct xfrm_user_mapping));
}
