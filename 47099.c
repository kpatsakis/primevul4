static void vhost_scsi_evt_work(struct vhost_work *work)
{
	struct vhost_scsi *vs = container_of(work, struct vhost_scsi,
					vs_event_work);
	struct vhost_virtqueue *vq = &vs->vqs[VHOST_SCSI_VQ_EVT].vq;
	struct vhost_scsi_evt *evt;
	struct llist_node *llnode;

	mutex_lock(&vq->mutex);
	llnode = llist_del_all(&vs->vs_event_list);
	while (llnode) {
		evt = llist_entry(llnode, struct vhost_scsi_evt, list);
		llnode = llist_next(llnode);
		vhost_scsi_do_evt_work(vs, evt);
		vhost_scsi_free_evt(vs, evt);
	}
	mutex_unlock(&vq->mutex);
}
