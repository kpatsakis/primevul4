static inline size_t xfrm_migrate_msgsize(int num_migrate, int with_kma)
{
	return NLMSG_ALIGN(sizeof(struct xfrm_userpolicy_id))
	      + (with_kma ? nla_total_size(sizeof(struct xfrm_kmaddress)) : 0)
	      + nla_total_size(sizeof(struct xfrm_user_migrate) * num_migrate)
	      + userpolicy_type_attrsize();
}
