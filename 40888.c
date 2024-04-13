static void purge_persistent_gnt(struct xen_blkif *blkif)
{
	struct persistent_gnt *persistent_gnt;
	struct rb_node *n;
	unsigned int num_clean, total;
	bool scan_used = false;
	struct rb_root *root;

	if (blkif->persistent_gnt_c < xen_blkif_max_pgrants ||
	    (blkif->persistent_gnt_c == xen_blkif_max_pgrants &&
	    !blkif->vbd.overflow_max_grants)) {
		return;
	}

	if (work_pending(&blkif->persistent_purge_work)) {
		pr_alert_ratelimited(DRV_PFX "Scheduled work from previous purge is still pending, cannot purge list\n");
		return;
	}

	num_clean = (xen_blkif_max_pgrants / 100) * LRU_PERCENT_CLEAN;
	num_clean = blkif->persistent_gnt_c - xen_blkif_max_pgrants + num_clean;
	num_clean = min(blkif->persistent_gnt_c, num_clean);
	if (num_clean >
	    (blkif->persistent_gnt_c -
	    atomic_read(&blkif->persistent_gnt_in_use)))
		return;

	/*
	 * At this point, we can assure that there will be no calls
         * to get_persistent_grant (because we are executing this code from
         * xen_blkif_schedule), there can only be calls to put_persistent_gnt,
         * which means that the number of currently used grants will go down,
         * but never up, so we will always be able to remove the requested
         * number of grants.
	 */

	total = num_clean;

	pr_debug(DRV_PFX "Going to purge %u persistent grants\n", num_clean);

	INIT_LIST_HEAD(&blkif->persistent_purge_list);
	root = &blkif->persistent_gnts;
purge_list:
	foreach_grant_safe(persistent_gnt, n, root, node) {
		BUG_ON(persistent_gnt->handle ==
			BLKBACK_INVALID_HANDLE);

		if (test_bit(PERSISTENT_GNT_ACTIVE, persistent_gnt->flags))
			continue;
		if (!scan_used &&
		    (test_bit(PERSISTENT_GNT_WAS_ACTIVE, persistent_gnt->flags)))
			continue;

		rb_erase(&persistent_gnt->node, root);
		list_add(&persistent_gnt->remove_node,
		         &blkif->persistent_purge_list);
		if (--num_clean == 0)
			goto finished;
	}
	/*
	 * If we get here it means we also need to start cleaning
	 * grants that were used since last purge in order to cope
	 * with the requested num
	 */
	if (!scan_used) {
		pr_debug(DRV_PFX "Still missing %u purged frames\n", num_clean);
		scan_used = true;
		goto purge_list;
	}
finished:
	/* Remove the "used" flag from all the persistent grants */
	foreach_grant_safe(persistent_gnt, n, root, node) {
		BUG_ON(persistent_gnt->handle ==
			BLKBACK_INVALID_HANDLE);
		clear_bit(PERSISTENT_GNT_WAS_ACTIVE, persistent_gnt->flags);
	}
	blkif->persistent_gnt_c -= (total - num_clean);
	blkif->vbd.overflow_max_grants = 0;

	/* We can defer this work */
	INIT_WORK(&blkif->persistent_purge_work, unmap_purged_grants);
	schedule_work(&blkif->persistent_purge_work);
	pr_debug(DRV_PFX "Purged %u/%u\n", (total - num_clean), total);
	return;
}
