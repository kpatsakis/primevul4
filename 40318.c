int rtnl_put_cacheinfo(struct sk_buff *skb, struct dst_entry *dst, u32 id,
		       long expires, u32 error)
{
	struct rta_cacheinfo ci = {
		.rta_lastuse = jiffies_delta_to_clock_t(jiffies - dst->lastuse),
		.rta_used = dst->__use,
		.rta_clntref = atomic_read(&(dst->__refcnt)),
		.rta_error = error,
		.rta_id =  id,
	};

	if (expires) {
		unsigned long clock;

		clock = jiffies_to_clock_t(abs(expires));
		clock = min_t(unsigned long, clock, INT_MAX);
		ci.rta_expires = (expires > 0) ? clock : -clock;
	}
	return nla_put(skb, RTA_CACHEINFO, sizeof(ci), &ci);
}
