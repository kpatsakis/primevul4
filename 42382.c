__ip_vs_dev_reset(struct ip_vs_dest *dest, struct net_device *dev)
{
	spin_lock_bh(&dest->dst_lock);
	if (dest->dst_cache && dest->dst_cache->dev == dev) {
		IP_VS_DBG_BUF(3, "Reset dev:%s dest %s:%u ,dest->refcnt=%d\n",
			      dev->name,
			      IP_VS_DBG_ADDR(dest->af, &dest->addr),
			      ntohs(dest->port),
			      atomic_read(&dest->refcnt));
		ip_vs_dst_reset(dest);
	}
	spin_unlock_bh(&dest->dst_lock);

}
