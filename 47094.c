static void vhost_scsi_done_inflight(struct kref *kref)
{
	struct vhost_scsi_inflight *inflight;

	inflight = container_of(kref, struct vhost_scsi_inflight, kref);
	complete(&inflight->comp);
}
