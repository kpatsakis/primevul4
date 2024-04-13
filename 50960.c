static int ffs_aio_cancel(struct kiocb *kiocb)
{
	struct ffs_io_data *io_data = kiocb->private;
	struct ffs_epfile *epfile = kiocb->ki_filp->private_data;
	int value;

	ENTER();

	spin_lock_irq(&epfile->ffs->eps_lock);

	if (likely(io_data && io_data->ep && io_data->req))
		value = usb_ep_dequeue(io_data->ep, io_data->req);
	else
		value = -EINVAL;

	spin_unlock_irq(&epfile->ffs->eps_lock);

	return value;
}
