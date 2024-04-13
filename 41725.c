static int copy_to_user_migrate(const struct xfrm_migrate *m, struct sk_buff *skb)
{
	struct xfrm_user_migrate um;

	memset(&um, 0, sizeof(um));
	um.proto = m->proto;
	um.mode = m->mode;
	um.reqid = m->reqid;
	um.old_family = m->old_family;
	memcpy(&um.old_daddr, &m->old_daddr, sizeof(um.old_daddr));
	memcpy(&um.old_saddr, &m->old_saddr, sizeof(um.old_saddr));
	um.new_family = m->new_family;
	memcpy(&um.new_daddr, &m->new_daddr, sizeof(um.new_daddr));
	memcpy(&um.new_saddr, &m->new_saddr, sizeof(um.new_saddr));

	return nla_put(skb, XFRMA_MIGRATE, sizeof(um), &um);
}
