void vop_host_uninit(struct vop_info *vi)
{
	struct list_head *pos, *tmp;
	struct vop_vdev *vdev;

	mutex_lock(&vi->vop_mutex);
	vop_virtio_reset_devices(vi);
	list_for_each_safe(pos, tmp, &vi->vdev_list) {
		vdev = list_entry(pos, struct vop_vdev, list);
		list_del(pos);
		reinit_completion(&vdev->destroy);
		mutex_unlock(&vi->vop_mutex);
		mutex_lock(&vdev->vdev_mutex);
		vop_virtio_del_device(vdev);
		vdev->deleted = true;
		mutex_unlock(&vdev->vdev_mutex);
		complete(&vdev->destroy);
		mutex_lock(&vi->vop_mutex);
	}
	mutex_unlock(&vi->vop_mutex);
	misc_deregister(&vi->miscdev);
}
