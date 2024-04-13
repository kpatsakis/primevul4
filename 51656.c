static int vop_virtio_copy_to_user(struct vop_vdev *vdev, void __user *ubuf,
				   size_t len, u64 daddr, size_t dlen,
				   int vr_idx)
{
	struct vop_device *vpdev = vdev->vpdev;
	void __iomem *dbuf = vpdev->hw_ops->ioremap(vpdev, daddr, len);
	struct vop_vringh *vvr = &vdev->vvr[vr_idx];
	struct vop_info *vi = dev_get_drvdata(&vpdev->dev);
	size_t dma_alignment = 1 << vi->dma_ch->device->copy_align;
	bool x200 = is_dma_copy_aligned(vi->dma_ch->device, 1, 1, 1);
	size_t dma_offset, partlen;
	int err;

	if (!VOP_USE_DMA) {
		if (copy_to_user(ubuf, (void __force *)dbuf, len)) {
			err = -EFAULT;
			dev_err(vop_dev(vdev), "%s %d err %d\n",
				__func__, __LINE__, err);
			goto err;
		}
		vdev->in_bytes += len;
		err = 0;
		goto err;
	}

	dma_offset = daddr - round_down(daddr, dma_alignment);
	daddr -= dma_offset;
	len += dma_offset;
	/*
	 * X100 uses DMA addresses as seen by the card so adding
	 * the aperture base is not required for DMA. However x200
	 * requires DMA addresses to be an offset into the bar so
	 * add the aperture base for x200.
	 */
	if (x200)
		daddr += vpdev->aper->pa;
	while (len) {
		partlen = min_t(size_t, len, VOP_INT_DMA_BUF_SIZE);
		err = vop_sync_dma(vdev, vvr->buf_da, daddr,
				   ALIGN(partlen, dma_alignment));
		if (err) {
			dev_err(vop_dev(vdev), "%s %d err %d\n",
				__func__, __LINE__, err);
			goto err;
		}
		if (copy_to_user(ubuf, vvr->buf + dma_offset,
				 partlen - dma_offset)) {
			err = -EFAULT;
			dev_err(vop_dev(vdev), "%s %d err %d\n",
				__func__, __LINE__, err);
			goto err;
		}
		daddr += partlen;
		ubuf += partlen;
		dbuf += partlen;
		vdev->in_bytes_dma += partlen;
		vdev->in_bytes += partlen;
		len -= partlen;
		dma_offset = 0;
	}
	err = 0;
err:
	vpdev->hw_ops->iounmap(vpdev, dbuf);
	dev_dbg(vop_dev(vdev),
		"%s: ubuf %p dbuf %p len 0x%lx vr_idx 0x%x\n",
		__func__, ubuf, dbuf, len, vr_idx);
	return err;
}
