static inline void vop_virtio_device_reset(struct vop_vdev *vdev)
{
	int i;

	dev_dbg(vop_dev(vdev), "%s: status %d device type %d RESET\n",
		__func__, vdev->dd->status, vdev->virtio_id);

	for (i = 0; i < vdev->dd->num_vq; i++)
		/*
		 * Avoid lockdep false positive. The + 1 is for the vop
		 * mutex which is held in the reset devices code path.
		 */
		mutex_lock_nested(&vdev->vvr[i].vr_mutex, i + 1);

	/* 0 status means "reset" */
	vdev->dd->status = 0;
	vdev->dc->vdev_reset = 0;
	vdev->dc->host_ack = 1;

	for (i = 0; i < vdev->dd->num_vq; i++) {
		struct vringh *vrh = &vdev->vvr[i].vrh;

		vdev->vvr[i].vring.info->avail_idx = 0;
		vrh->completed = 0;
		vrh->last_avail_idx = 0;
		vrh->last_used_idx = 0;
	}

	for (i = 0; i < vdev->dd->num_vq; i++)
		mutex_unlock(&vdev->vvr[i].vr_mutex);
}
