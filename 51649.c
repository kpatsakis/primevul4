static int vop_open(struct inode *inode, struct file *f)
{
	struct vop_vdev *vdev;
	struct vop_info *vi = container_of(f->private_data,
		struct vop_info, miscdev);

	vdev = kzalloc(sizeof(*vdev), GFP_KERNEL);
	if (!vdev)
		return -ENOMEM;
	vdev->vi = vi;
	mutex_init(&vdev->vdev_mutex);
	f->private_data = vdev;
	init_completion(&vdev->destroy);
	complete(&vdev->destroy);
	return 0;
}
