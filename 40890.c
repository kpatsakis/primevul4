static void put_persistent_gnt(struct xen_blkif *blkif,
                               struct persistent_gnt *persistent_gnt)
{
	if(!test_bit(PERSISTENT_GNT_ACTIVE, persistent_gnt->flags))
	          pr_alert_ratelimited(DRV_PFX " freeing a grant already unused");
	set_bit(PERSISTENT_GNT_WAS_ACTIVE, persistent_gnt->flags);
	clear_bit(PERSISTENT_GNT_ACTIVE, persistent_gnt->flags);
	atomic_dec(&blkif->persistent_gnt_in_use);
}
