static unsigned int vop_poll(struct file *f, poll_table *wait)
{
	struct vop_vdev *vdev = f->private_data;
	int mask = 0;

	mutex_lock(&vdev->vdev_mutex);
	if (vop_vdev_inited(vdev)) {
		mask = POLLERR;
		goto done;
	}
	poll_wait(f, &vdev->waitq, wait);
	if (vop_vdev_inited(vdev)) {
		mask = POLLERR;
	} else if (vdev->poll_wake) {
		vdev->poll_wake = 0;
		mask = POLLIN | POLLOUT;
	}
done:
	mutex_unlock(&vdev->vdev_mutex);
	return mask;
}
