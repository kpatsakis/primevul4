static ssize_t ffs_epfile_io(struct file *file, struct ffs_io_data *io_data)
{
	struct ffs_epfile *epfile = file->private_data;
	struct usb_request *req;
	struct ffs_ep *ep;
	char *data = NULL;
	ssize_t ret, data_len = -EINVAL;
	int halt;

	/* Are we still active? */
	if (WARN_ON(epfile->ffs->state != FFS_ACTIVE))
		return -ENODEV;

	/* Wait for endpoint to be enabled */
	ep = epfile->ep;
	if (!ep) {
		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		ret = wait_event_interruptible(epfile->wait, (ep = epfile->ep));
		if (ret)
			return -EINTR;
	}

	/* Do we halt? */
	halt = (!io_data->read == !epfile->in);
	if (halt && epfile->isoc)
		return -EINVAL;

	/* Allocate & copy */
	if (!halt) {
		/*
		 * if we _do_ wait above, the epfile->ffs->gadget might be NULL
		 * before the waiting completes, so do not assign to 'gadget'
		 * earlier
		 */
		struct usb_gadget *gadget = epfile->ffs->gadget;
		size_t copied;

		spin_lock_irq(&epfile->ffs->eps_lock);
		/* In the meantime, endpoint got disabled or changed. */
		if (epfile->ep != ep) {
			spin_unlock_irq(&epfile->ffs->eps_lock);
			return -ESHUTDOWN;
		}
		data_len = iov_iter_count(&io_data->data);
		/*
		 * Controller may require buffer size to be aligned to
		 * maxpacketsize of an out endpoint.
		 */
		if (io_data->read)
			data_len = usb_ep_align_maybe(gadget, ep->ep, data_len);
		spin_unlock_irq(&epfile->ffs->eps_lock);

		data = kmalloc(data_len, GFP_KERNEL);
		if (unlikely(!data))
			return -ENOMEM;
		if (!io_data->read) {
			copied = copy_from_iter(data, data_len, &io_data->data);
			if (copied != data_len) {
				ret = -EFAULT;
				goto error;
			}
		}
	}

	/* We will be using request */
	ret = ffs_mutex_lock(&epfile->mutex, file->f_flags & O_NONBLOCK);
	if (unlikely(ret))
		goto error;

	spin_lock_irq(&epfile->ffs->eps_lock);

	if (epfile->ep != ep) {
		/* In the meantime, endpoint got disabled or changed. */
		ret = -ESHUTDOWN;
	} else if (halt) {
		/* Halt */
		if (likely(epfile->ep == ep) && !WARN_ON(!ep->ep))
			usb_ep_set_halt(ep->ep);
		ret = -EBADMSG;
	} else if (unlikely(data_len == -EINVAL)) {
		/*
		 * Sanity Check: even though data_len can't be used
		 * uninitialized at the time I write this comment, some
		 * compilers complain about this situation.
		 * In order to keep the code clean from warnings, data_len is
		 * being initialized to -EINVAL during its declaration, which
		 * means we can't rely on compiler anymore to warn no future
		 * changes won't result in data_len being used uninitialized.
		 * For such reason, we're adding this redundant sanity check
		 * here.
		 */
		WARN(1, "%s: data_len == -EINVAL\n", __func__);
		ret = -EINVAL;
	} else if (!io_data->aio) {
		DECLARE_COMPLETION_ONSTACK(done);
		bool interrupted = false;

		req = ep->req;
		req->buf      = data;
		req->length   = data_len;

		req->context  = &done;
		req->complete = ffs_epfile_io_complete;

		ret = usb_ep_queue(ep->ep, req, GFP_ATOMIC);
		if (unlikely(ret < 0))
			goto error_lock;

		spin_unlock_irq(&epfile->ffs->eps_lock);

		if (unlikely(wait_for_completion_interruptible(&done))) {
			/*
			 * To avoid race condition with ffs_epfile_io_complete,
			 * dequeue the request first then check
			 * status. usb_ep_dequeue API should guarantee no race
			 * condition with req->complete callback.
			 */
			usb_ep_dequeue(ep->ep, req);
			interrupted = ep->status < 0;
		}

		/*
		 * XXX We may end up silently droping data here.  Since data_len
		 * (i.e. req->length) may be bigger than len (after being
		 * rounded up to maxpacketsize), we may end up with more data
		 * then user space has space for.
		 */
		ret = interrupted ? -EINTR : ep->status;
		if (io_data->read && ret > 0) {
			ret = copy_to_iter(data, ret, &io_data->data);
			if (!ret)
				ret = -EFAULT;
		}
		goto error_mutex;
	} else if (!(req = usb_ep_alloc_request(ep->ep, GFP_KERNEL))) {
		ret = -ENOMEM;
	} else {
		req->buf      = data;
		req->length   = data_len;

		io_data->buf = data;
		io_data->ep = ep->ep;
		io_data->req = req;
		io_data->ffs = epfile->ffs;

		req->context  = io_data;
		req->complete = ffs_epfile_async_io_complete;

		ret = usb_ep_queue(ep->ep, req, GFP_ATOMIC);
		if (unlikely(ret)) {
			usb_ep_free_request(ep->ep, req);
			goto error_lock;
		}

		ret = -EIOCBQUEUED;
		/*
		 * Do not kfree the buffer in this function.  It will be freed
		 * by ffs_user_copy_worker.
		 */
		data = NULL;
	}

error_lock:
	spin_unlock_irq(&epfile->ffs->eps_lock);
error_mutex:
	mutex_unlock(&epfile->mutex);
error:
	kfree(data);
	return ret;
}
