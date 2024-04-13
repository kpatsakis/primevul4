static void async_completed(struct urb *urb)
{
	struct async *as = urb->context;
	struct usb_dev_state *ps = as->ps;
	struct siginfo sinfo;
	struct pid *pid = NULL;
	u32 secid = 0;
	const struct cred *cred = NULL;
	int signr;

	spin_lock(&ps->lock);
	list_move_tail(&as->asynclist, &ps->async_completed);
	as->status = urb->status;
	signr = as->signr;
	if (signr) {
		memset(&sinfo, 0, sizeof(sinfo));
		sinfo.si_signo = as->signr;
		sinfo.si_errno = as->status;
		sinfo.si_code = SI_ASYNCIO;
		sinfo.si_addr = as->userurb;
		pid = get_pid(as->pid);
		cred = get_cred(as->cred);
		secid = as->secid;
	}
	snoop(&urb->dev->dev, "urb complete\n");
	snoop_urb(urb->dev, as->userurb, urb->pipe, urb->actual_length,
			as->status, COMPLETE, NULL, 0);
	if ((urb->transfer_flags & URB_DIR_MASK) == URB_DIR_IN)
		snoop_urb_data(urb, urb->actual_length);

	if (as->status < 0 && as->bulk_addr && as->status != -ECONNRESET &&
			as->status != -ENOENT)
		cancel_bulk_urbs(ps, as->bulk_addr);
	spin_unlock(&ps->lock);

	if (signr) {
		kill_pid_info_as_cred(sinfo.si_signo, &sinfo, pid, cred, secid);
		put_pid(pid);
		put_cred(cred);
	}

	wake_up(&ps->wait);
}
