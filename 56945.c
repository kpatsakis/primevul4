static int add_persistent_gnt(struct xen_blkif_ring *ring,
			       struct persistent_gnt *persistent_gnt)
{
	struct rb_node **new = NULL, *parent = NULL;
	struct persistent_gnt *this;
	struct xen_blkif *blkif = ring->blkif;

	if (ring->persistent_gnt_c >= xen_blkif_max_pgrants) {
		if (!blkif->vbd.overflow_max_grants)
			blkif->vbd.overflow_max_grants = 1;
		return -EBUSY;
	}
	/* Figure out where to put new node */
	new = &ring->persistent_gnts.rb_node;
	while (*new) {
		this = container_of(*new, struct persistent_gnt, node);

		parent = *new;
		if (persistent_gnt->gnt < this->gnt)
			new = &((*new)->rb_left);
		else if (persistent_gnt->gnt > this->gnt)
			new = &((*new)->rb_right);
		else {
			pr_alert_ratelimited("trying to add a gref that's already in the tree\n");
			return -EINVAL;
		}
	}

	bitmap_zero(persistent_gnt->flags, PERSISTENT_GNT_FLAGS_SIZE);
	set_bit(PERSISTENT_GNT_ACTIVE, persistent_gnt->flags);
	/* Add new node and rebalance tree. */
	rb_link_node(&(persistent_gnt->node), parent, new);
	rb_insert_color(&(persistent_gnt->node), &ring->persistent_gnts);
	ring->persistent_gnt_c++;
	atomic_inc(&ring->persistent_gnt_in_use);
	return 0;
}
