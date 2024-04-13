vhost_scsi_do_plug(struct vhost_scsi_tpg *tpg,
		  struct se_lun *lun, bool plug)
{

	struct vhost_scsi *vs = tpg->vhost_scsi;
	struct vhost_virtqueue *vq;
	u32 reason;

	if (!vs)
		return;

	mutex_lock(&vs->dev.mutex);

	if (plug)
		reason = VIRTIO_SCSI_EVT_RESET_RESCAN;
	else
		reason = VIRTIO_SCSI_EVT_RESET_REMOVED;

	vq = &vs->vqs[VHOST_SCSI_VQ_EVT].vq;
	mutex_lock(&vq->mutex);
	if (vhost_has_feature(vq, VIRTIO_SCSI_F_HOTPLUG))
		vhost_scsi_send_evt(vs, tpg, lun,
				   VIRTIO_SCSI_T_TRANSPORT_RESET, reason);
	mutex_unlock(&vq->mutex);
	mutex_unlock(&vs->dev.mutex);
}
