static int vop_release(struct inode *inode, struct file *f)
{
	struct vop_vdev *vdev = f->private_data, *vdev_tmp;
	struct vop_info *vi = vdev->vi;
	struct list_head *pos, *tmp;
	bool found = false;

	mutex_lock(&vdev->vdev_mutex);
	if (vdev->deleted)
		goto unlock;
	mutex_lock(&vi->vop_mutex);
	list_for_each_safe(pos, tmp, &vi->vdev_list) {
		vdev_tmp = list_entry(pos, struct vop_vdev, list);
		if (vdev == vdev_tmp) {
			vop_virtio_del_device(vdev);
			list_del(pos);
			found = true;
			break;
		}
	}
	mutex_unlock(&vi->vop_mutex);
unlock:
	mutex_unlock(&vdev->vdev_mutex);
	if (!found)
		wait_for_completion(&vdev->destroy);
	f->private_data = NULL;
	kfree(vdev);
	return 0;
}
