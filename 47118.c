static void vhost_scsi_init_inflight(struct vhost_scsi *vs,
				    struct vhost_scsi_inflight *old_inflight[])
{
	struct vhost_scsi_inflight *new_inflight;
	struct vhost_virtqueue *vq;
	int idx, i;

	for (i = 0; i < VHOST_SCSI_MAX_VQ; i++) {
		vq = &vs->vqs[i].vq;

		mutex_lock(&vq->mutex);

		/* store old infight */
		idx = vs->vqs[i].inflight_idx;
		if (old_inflight)
			old_inflight[i] = &vs->vqs[i].inflights[idx];

		/* setup new infight */
		vs->vqs[i].inflight_idx = idx ^ 1;
		new_inflight = &vs->vqs[i].inflights[idx ^ 1];
		kref_init(&new_inflight->kref);
		init_completion(&new_inflight->comp);

		mutex_unlock(&vq->mutex);
	}
}
