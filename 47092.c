vhost_scsi_do_evt_work(struct vhost_scsi *vs, struct vhost_scsi_evt *evt)
{
	struct vhost_virtqueue *vq = &vs->vqs[VHOST_SCSI_VQ_EVT].vq;
	struct virtio_scsi_event *event = &evt->event;
	struct virtio_scsi_event __user *eventp;
	unsigned out, in;
	int head, ret;

	if (!vq->private_data) {
		vs->vs_events_missed = true;
		return;
	}

again:
	vhost_disable_notify(&vs->dev, vq);
	head = vhost_get_vq_desc(vq, vq->iov,
			ARRAY_SIZE(vq->iov), &out, &in,
			NULL, NULL);
	if (head < 0) {
		vs->vs_events_missed = true;
		return;
	}
	if (head == vq->num) {
		if (vhost_enable_notify(&vs->dev, vq))
			goto again;
		vs->vs_events_missed = true;
		return;
	}

	if ((vq->iov[out].iov_len != sizeof(struct virtio_scsi_event))) {
		vq_err(vq, "Expecting virtio_scsi_event, got %zu bytes\n",
				vq->iov[out].iov_len);
		vs->vs_events_missed = true;
		return;
	}

	if (vs->vs_events_missed) {
		event->event |= cpu_to_vhost32(vq, VIRTIO_SCSI_T_EVENTS_MISSED);
		vs->vs_events_missed = false;
	}

	eventp = vq->iov[out].iov_base;
	ret = __copy_to_user(eventp, event, sizeof(*event));
	if (!ret)
		vhost_add_used_and_signal(&vs->dev, vq, head, 0);
	else
		vq_err(vq, "Faulted on vhost_scsi_send_event\n");
}
