static void usbdev_remove(struct usb_device *udev)
{
	struct usb_dev_state *ps;
	struct siginfo sinfo;

	while (!list_empty(&udev->filelist)) {
		ps = list_entry(udev->filelist.next, struct usb_dev_state, list);
		destroy_all_async(ps);
		wake_up_all(&ps->wait);
		list_del_init(&ps->list);
		if (ps->discsignr) {
			memset(&sinfo, 0, sizeof(sinfo));
			sinfo.si_signo = ps->discsignr;
			sinfo.si_errno = EPIPE;
			sinfo.si_code = SI_ASYNCIO;
			sinfo.si_addr = ps->disccontext;
			kill_pid_info_as_cred(ps->discsignr, &sinfo,
					ps->disc_pid, ps->cred, ps->secid);
		}
	}
}
