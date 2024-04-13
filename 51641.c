static int _vop_virtio_copy(struct vop_vdev *vdev, struct mic_copy_desc *copy)
{
	int ret = 0;
	u32 iovcnt = copy->iovcnt;
	struct iovec iov;
	struct iovec __user *u_iov = copy->iov;
	void __user *ubuf = NULL;
	struct vop_vringh *vvr = &vdev->vvr[copy->vr_idx];
	struct vringh_kiov *riov = &vvr->riov;
	struct vringh_kiov *wiov = &vvr->wiov;
	struct vringh *vrh = &vvr->vrh;
	u16 *head = &vvr->head;
	struct mic_vring *vr = &vvr->vring;
	size_t len = 0, out_len;

	copy->out_len = 0;
	/* Fetch a new IOVEC if all previous elements have been processed */
	if (riov->i == riov->used && wiov->i == wiov->used) {
		ret = vringh_getdesc_kern(vrh, riov, wiov,
					  head, GFP_KERNEL);
		/* Check if there are available descriptors */
		if (ret <= 0)
			return ret;
	}
	while (iovcnt) {
		if (!len) {
			/* Copy over a new iovec from user space. */
			ret = copy_from_user(&iov, u_iov, sizeof(*u_iov));
			if (ret) {
				ret = -EINVAL;
				dev_err(vop_dev(vdev), "%s %d err %d\n",
					__func__, __LINE__, ret);
				break;
			}
			len = iov.iov_len;
			ubuf = iov.iov_base;
		}
		/* Issue all the read descriptors first */
		ret = vop_vringh_copy(vdev, riov, ubuf, len,
				      MIC_VRINGH_READ, copy->vr_idx, &out_len);
		if (ret) {
			dev_err(vop_dev(vdev), "%s %d err %d\n",
				__func__, __LINE__, ret);
			break;
		}
		len -= out_len;
		ubuf += out_len;
		copy->out_len += out_len;
		/* Issue the write descriptors next */
		ret = vop_vringh_copy(vdev, wiov, ubuf, len,
				      !MIC_VRINGH_READ, copy->vr_idx, &out_len);
		if (ret) {
			dev_err(vop_dev(vdev), "%s %d err %d\n",
				__func__, __LINE__, ret);
			break;
		}
		len -= out_len;
		ubuf += out_len;
		copy->out_len += out_len;
		if (!len) {
			/* One user space iovec is now completed */
			iovcnt--;
			u_iov++;
		}
		/* Exit loop if all elements in KIOVs have been processed. */
		if (riov->i == riov->used && wiov->i == wiov->used)
			break;
	}
	/*
	 * Update the used ring if a descriptor was available and some data was
	 * copied in/out and the user asked for a used ring update.
	 */
	if (*head != USHRT_MAX && copy->out_len && copy->update_used) {
		u32 total = 0;

		/* Determine the total data consumed */
		total += vop_vringh_iov_consumed(riov);
		total += vop_vringh_iov_consumed(wiov);
		vringh_complete_kern(vrh, *head, total);
		*head = USHRT_MAX;
		if (vringh_need_notify_kern(vrh) > 0)
			vringh_notify(vrh);
		vringh_kiov_cleanup(riov);
		vringh_kiov_cleanup(wiov);
		/* Update avail idx for user space */
		vr->info->avail_idx = vrh->last_avail_idx;
	}
	return ret;
}
