sg_remove_sfp_usercontext(struct work_struct *work)
{
	struct sg_fd *sfp = container_of(work, struct sg_fd, ew.work);
	struct sg_device *sdp = sfp->parentdp;
	Sg_request *srp;
	unsigned long iflags;

	/* Cleanup any responses which were never read(). */
	write_lock_irqsave(&sfp->rq_list_lock, iflags);
	while (!list_empty(&sfp->rq_list)) {
		srp = list_first_entry(&sfp->rq_list, Sg_request, entry);
		sg_finish_rem_req(srp);
		list_del(&srp->entry);
		srp->parentfp = NULL;
	}
	write_unlock_irqrestore(&sfp->rq_list_lock, iflags);

	if (sfp->reserve.bufflen > 0) {
		SCSI_LOG_TIMEOUT(6, sg_printk(KERN_INFO, sdp,
				"sg_remove_sfp:    bufflen=%d, k_use_sg=%d\n",
				(int) sfp->reserve.bufflen,
				(int) sfp->reserve.k_use_sg));
		sg_remove_scat(sfp, &sfp->reserve);
	}

	SCSI_LOG_TIMEOUT(6, sg_printk(KERN_INFO, sdp,
			"sg_remove_sfp: sfp=0x%p\n", sfp));
	kfree(sfp);

	scsi_device_put(sdp->device);
	kref_put(&sdp->d_ref, sg_device_destroy);
	module_put(THIS_MODULE);
}
