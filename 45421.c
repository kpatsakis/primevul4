struct net_device *atrtr_get_dev(struct atalk_addr *sa)
{
	struct atalk_route *atr = atrtr_find(sa);
	return atr ? atr->dev : NULL;
}
