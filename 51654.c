static inline int vop_verify_copy_args(struct vop_vdev *vdev,
				       struct mic_copy_desc *copy)
{
	if (!vdev || copy->vr_idx >= vdev->dd->num_vq)
		return -EINVAL;
	return 0;
}
