void vhost_ubuf_put_and_wait(struct vhost_ubuf_ref *ubufs)
{
	kref_put(&ubufs->kref, vhost_zerocopy_done_signal);
	wait_event(ubufs->wait, !atomic_read(&ubufs->kref.refcount));
	kfree(ubufs);
}
