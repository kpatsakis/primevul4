static int tcp_repair_set_window(struct tcp_sock *tp, char __user *optbuf, int len)
{
	struct tcp_repair_window opt;

	if (!tp->repair)
		return -EPERM;

	if (len != sizeof(opt))
		return -EINVAL;

	if (copy_from_user(&opt, optbuf, sizeof(opt)))
		return -EFAULT;

	if (opt.max_window < opt.snd_wnd)
		return -EINVAL;

	if (after(opt.snd_wl1, tp->rcv_nxt + opt.rcv_wnd))
		return -EINVAL;

	if (after(opt.rcv_wup, tp->rcv_nxt))
		return -EINVAL;

	tp->snd_wl1	= opt.snd_wl1;
	tp->snd_wnd	= opt.snd_wnd;
	tp->max_window	= opt.max_window;

	tp->rcv_wnd	= opt.rcv_wnd;
	tp->rcv_wup	= opt.rcv_wup;

	return 0;
}
