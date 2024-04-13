static void vhost_zerocopy_done_signal(struct kref *kref)
{
	struct vhost_ubuf_ref *ubufs = container_of(kref, struct vhost_ubuf_ref,
						    kref);
	wake_up(&ubufs->wait);
}
