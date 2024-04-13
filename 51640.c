static void _vop_notify(struct vringh *vrh)
{
	struct vop_vringh *vvrh = container_of(vrh, struct vop_vringh, vrh);
	struct vop_vdev *vdev = vvrh->vdev;
	struct vop_device *vpdev = vdev->vpdev;
	s8 db = vdev->dc->h2c_vdev_db;

	if (db != -1)
		vpdev->hw_ops->send_intr(vpdev, db);
}
