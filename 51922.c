static unsigned int ppp_poll(struct file *file, poll_table *wait)
{
	struct ppp_file *pf = file->private_data;
	unsigned int mask;

	if (!pf)
		return 0;
	poll_wait(file, &pf->rwait, wait);
	mask = POLLOUT | POLLWRNORM;
	if (skb_peek(&pf->rq))
		mask |= POLLIN | POLLRDNORM;
	if (pf->dead)
		mask |= POLLHUP;
	else if (pf->kind == INTERFACE) {
		/* see comment in ppp_read */
		struct ppp *ppp = PF_TO_PPP(pf);

		ppp_recv_lock(ppp);
		if (ppp->n_channels == 0 &&
		    (ppp->flags & SC_LOOP_TRAFFIC) == 0)
			mask |= POLLIN | POLLRDNORM;
		ppp_recv_unlock(ppp);
	}

	return mask;
}
