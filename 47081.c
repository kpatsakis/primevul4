vhost_scsi_allocate_evt(struct vhost_scsi *vs,
		       u32 event, u32 reason)
{
	struct vhost_virtqueue *vq = &vs->vqs[VHOST_SCSI_VQ_EVT].vq;
	struct vhost_scsi_evt *evt;

	if (vs->vs_events_nr > VHOST_SCSI_MAX_EVENT) {
		vs->vs_events_missed = true;
		return NULL;
	}

	evt = kzalloc(sizeof(*evt), GFP_KERNEL);
	if (!evt) {
		vq_err(vq, "Failed to allocate vhost_scsi_evt\n");
		vs->vs_events_missed = true;
		return NULL;
	}

	evt->event.event = cpu_to_vhost32(vq, event);
	evt->event.reason = cpu_to_vhost32(vq, reason);
	vs->vs_events_nr++;

	return evt;
}
