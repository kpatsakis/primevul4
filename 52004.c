static int get_urb32(struct usbdevfs_urb *kurb,
		     struct usbdevfs_urb32 __user *uurb)
{
	__u32  uptr;
	if (!access_ok(VERIFY_READ, uurb, sizeof(*uurb)) ||
	    __get_user(kurb->type, &uurb->type) ||
	    __get_user(kurb->endpoint, &uurb->endpoint) ||
	    __get_user(kurb->status, &uurb->status) ||
	    __get_user(kurb->flags, &uurb->flags) ||
	    __get_user(kurb->buffer_length, &uurb->buffer_length) ||
	    __get_user(kurb->actual_length, &uurb->actual_length) ||
	    __get_user(kurb->start_frame, &uurb->start_frame) ||
	    __get_user(kurb->number_of_packets, &uurb->number_of_packets) ||
	    __get_user(kurb->error_count, &uurb->error_count) ||
	    __get_user(kurb->signr, &uurb->signr))
		return -EFAULT;

	if (__get_user(uptr, &uurb->buffer))
		return -EFAULT;
	kurb->buffer = compat_ptr(uptr);
	if (__get_user(uptr, &uurb->usercontext))
		return -EFAULT;
	kurb->usercontext = compat_ptr(uptr);

	return 0;
}
