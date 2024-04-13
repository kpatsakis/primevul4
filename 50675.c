static void packet_dev_mclist_delete(struct net_device *dev,
				     struct packet_mclist **mlp)
{
	struct packet_mclist *ml;

	while ((ml = *mlp) != NULL) {
		if (ml->ifindex == dev->ifindex) {
			packet_dev_mc(dev, ml, -1);
			*mlp = ml->next;
			kfree(ml);
		} else
			mlp = &ml->next;
	}
}
