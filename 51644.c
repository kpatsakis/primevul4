static inline struct device *vop_dev(struct vop_vdev *vdev)
{
	return vdev->vpdev->dev.parent;
}
