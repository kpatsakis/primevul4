static ssize_t ffs_epfile_read_iter(struct kiocb *kiocb, struct iov_iter *to)
{
	struct ffs_io_data io_data, *p = &io_data;
	ssize_t res;

	ENTER();

	if (!is_sync_kiocb(kiocb)) {
		p = kmalloc(sizeof(io_data), GFP_KERNEL);
		if (unlikely(!p))
			return -ENOMEM;
		p->aio = true;
	} else {
		p->aio = false;
	}

	p->read = true;
	p->kiocb = kiocb;
	if (p->aio) {
		p->to_free = dup_iter(&p->data, to, GFP_KERNEL);
		if (!p->to_free) {
			kfree(p);
			return -ENOMEM;
		}
	} else {
		p->data = *to;
		p->to_free = NULL;
	}
	p->mm = current->mm;

	kiocb->private = p;

	if (p->aio)
		kiocb_set_cancel_fn(kiocb, ffs_aio_cancel);

	res = ffs_epfile_io(kiocb->ki_filp, p);
	if (res == -EIOCBQUEUED)
		return res;

	if (p->aio) {
		kfree(p->to_free);
		kfree(p);
	} else {
		*to = p->data;
	}
	return res;
}
