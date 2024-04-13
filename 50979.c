static void ffs_epfile_async_io_complete(struct usb_ep *_ep,
					 struct usb_request *req)
{
	struct ffs_io_data *io_data = req->context;

	ENTER();

	INIT_WORK(&io_data->work, ffs_user_copy_worker);
	schedule_work(&io_data->work);
}
