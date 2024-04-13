static void vhost_scsi_free_evt(struct vhost_scsi *vs, struct vhost_scsi_evt *evt)
{
	vs->vs_events_nr--;
	kfree(evt);
}
