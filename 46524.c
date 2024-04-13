sg_remove_scat(Sg_fd * sfp, Sg_scatter_hold * schp)
{
	SCSI_LOG_TIMEOUT(4, sg_printk(KERN_INFO, sfp->parentdp,
			 "sg_remove_scat: k_use_sg=%d\n", schp->k_use_sg));
	if (schp->pages && schp->sglist_len > 0) {
		if (!schp->dio_in_use) {
			int k;

			for (k = 0; k < schp->k_use_sg && schp->pages[k]; k++) {
				SCSI_LOG_TIMEOUT(5,
					sg_printk(KERN_INFO, sfp->parentdp,
					"sg_remove_scat: k=%d, pg=0x%p\n",
					k, schp->pages[k]));
				__free_pages(schp->pages[k], schp->page_order);
			}

			kfree(schp->pages);
		}
	}
	memset(schp, 0, sizeof (*schp));
}
