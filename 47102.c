static void vhost_scsi_flush_vq(struct vhost_scsi *vs, int index)
{
	vhost_poll_flush(&vs->vqs[index].vq.poll);
}
