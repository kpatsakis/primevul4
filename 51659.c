static void vop_virtio_reset_devices(struct vop_info *vi)
{
	struct list_head *pos, *tmp;
	struct vop_vdev *vdev;

	list_for_each_safe(pos, tmp, &vi->vdev_list) {
		vdev = list_entry(pos, struct vop_vdev, list);
		vop_virtio_device_reset(vdev);
		vdev->poll_wake = 1;
		wake_up(&vdev->waitq);
	}
}
