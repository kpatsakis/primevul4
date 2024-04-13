static void put_persistent_gnt(struct xen_blkif_ring *ring,
                               struct persistent_gnt *persistent_gnt)
{
	if(!test_bit(PERSISTENT_GNT_ACTIVE, persistent_gnt->flags))
		pr_alert_ratelimited("freeing a grant already unused\n");
	set_bit(PERSISTENT_GNT_WAS_ACTIVE, persistent_gnt->flags);
	clear_bit(PERSISTENT_GNT_ACTIVE, persistent_gnt->flags);
	atomic_dec(&ring->persistent_gnt_in_use);
}
