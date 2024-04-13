ppp_set_compress(struct ppp *ppp, unsigned long arg)
{
	int err;
	struct compressor *cp, *ocomp;
	struct ppp_option_data data;
	void *state, *ostate;
	unsigned char ccp_option[CCP_MAX_OPTION_LENGTH];

	err = -EFAULT;
	if (copy_from_user(&data, (void __user *) arg, sizeof(data)))
		goto out;
	if (data.length > CCP_MAX_OPTION_LENGTH)
		goto out;
	if (copy_from_user(ccp_option, (void __user *) data.ptr, data.length))
		goto out;

	err = -EINVAL;
	if (data.length < 2 || ccp_option[1] < 2 || ccp_option[1] > data.length)
		goto out;

	cp = try_then_request_module(
		find_compressor(ccp_option[0]),
		"ppp-compress-%d", ccp_option[0]);
	if (!cp)
		goto out;

	err = -ENOBUFS;
	if (data.transmit) {
		state = cp->comp_alloc(ccp_option, data.length);
		if (state) {
			ppp_xmit_lock(ppp);
			ppp->xstate &= ~SC_COMP_RUN;
			ocomp = ppp->xcomp;
			ostate = ppp->xc_state;
			ppp->xcomp = cp;
			ppp->xc_state = state;
			ppp_xmit_unlock(ppp);
			if (ostate) {
				ocomp->comp_free(ostate);
				module_put(ocomp->owner);
			}
			err = 0;
		} else
			module_put(cp->owner);

	} else {
		state = cp->decomp_alloc(ccp_option, data.length);
		if (state) {
			ppp_recv_lock(ppp);
			ppp->rstate &= ~SC_DECOMP_RUN;
			ocomp = ppp->rcomp;
			ostate = ppp->rc_state;
			ppp->rcomp = cp;
			ppp->rc_state = state;
			ppp_recv_unlock(ppp);
			if (ostate) {
				ocomp->decomp_free(ostate);
				module_put(ocomp->owner);
			}
			err = 0;
		} else
			module_put(cp->owner);
	}

 out:
	return err;
}
