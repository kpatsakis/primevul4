vhost_scsi_send_bad_target(struct vhost_scsi *vs,
			   struct vhost_virtqueue *vq,
			   int head, unsigned out)
{
	struct virtio_scsi_cmd_resp __user *resp;
	struct virtio_scsi_cmd_resp rsp;
	int ret;

	memset(&rsp, 0, sizeof(rsp));
	rsp.response = VIRTIO_SCSI_S_BAD_TARGET;
	resp = vq->iov[out].iov_base;
	ret = __copy_to_user(resp, &rsp, sizeof(rsp));
	if (!ret)
		vhost_add_used_and_signal(&vs->dev, vq, head, 0);
	else
		pr_err("Faulted on virtio_scsi_cmd_resp\n");
}
