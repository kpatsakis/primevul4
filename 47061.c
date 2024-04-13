static bool virtnet_send_command(struct virtnet_info *vi, u8 class, u8 cmd,
				 struct scatterlist *out)
{
	struct scatterlist *sgs[4], hdr, stat;
	struct virtio_net_ctrl_hdr ctrl;
	virtio_net_ctrl_ack status = ~0;
	unsigned out_num = 0, tmp;

	/* Caller should know better */
	BUG_ON(!virtio_has_feature(vi->vdev, VIRTIO_NET_F_CTRL_VQ));

	ctrl.class = class;
	ctrl.cmd = cmd;
	/* Add header */
	sg_init_one(&hdr, &ctrl, sizeof(ctrl));
	sgs[out_num++] = &hdr;

	if (out)
		sgs[out_num++] = out;

	/* Add return status. */
	sg_init_one(&stat, &status, sizeof(status));
	sgs[out_num] = &stat;

	BUG_ON(out_num + 1 > ARRAY_SIZE(sgs));
	virtqueue_add_sgs(vi->cvq, sgs, out_num, 1, vi, GFP_ATOMIC);

	if (unlikely(!virtqueue_kick(vi->cvq)))
		return status == VIRTIO_NET_OK;

	/* Spin for a response, the kick causes an ioport write, trapping
	 * into the hypervisor, so the request should be handled immediately.
	 */
	while (!virtqueue_get_buf(vi->cvq, &tmp) &&
	       !virtqueue_is_broken(vi->cvq))
		cpu_relax();

	return status == VIRTIO_NET_OK;
}
