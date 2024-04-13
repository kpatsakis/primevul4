static int __must_check ax25_connect(struct socket *sock,
	struct sockaddr *uaddr, int addr_len, int flags)
{
	struct sock *sk = sock->sk;
	ax25_cb *ax25 = ax25_sk(sk), *ax25t;
	struct full_sockaddr_ax25 *fsa = (struct full_sockaddr_ax25 *)uaddr;
	ax25_digi *digi = NULL;
	int ct = 0, err = 0;

	/*
	 * some sanity checks. code further down depends on this
	 */

	if (addr_len == sizeof(struct sockaddr_ax25))
		/* support for this will go away in early 2.5.x
		 * ax25_connect(): uses obsolete socket structure
		 */
		;
	else if (addr_len != sizeof(struct full_sockaddr_ax25))
		/* support for old structure may go away some time
		 * ax25_connect(): uses old (6 digipeater) socket structure.
		 */
		if ((addr_len < sizeof(struct sockaddr_ax25) + sizeof(ax25_address) * 6) ||
		    (addr_len > sizeof(struct full_sockaddr_ax25)))
			return -EINVAL;


	if (fsa->fsa_ax25.sax25_family != AF_AX25)
		return -EINVAL;

	lock_sock(sk);

	/* deal with restarts */
	if (sock->state == SS_CONNECTING) {
		switch (sk->sk_state) {
		case TCP_SYN_SENT: /* still trying */
			err = -EINPROGRESS;
			goto out_release;

		case TCP_ESTABLISHED: /* connection established */
			sock->state = SS_CONNECTED;
			goto out_release;

		case TCP_CLOSE: /* connection refused */
			sock->state = SS_UNCONNECTED;
			err = -ECONNREFUSED;
			goto out_release;
		}
	}

	if (sk->sk_state == TCP_ESTABLISHED && sk->sk_type == SOCK_SEQPACKET) {
		err = -EISCONN;	/* No reconnect on a seqpacket socket */
		goto out_release;
	}

	sk->sk_state   = TCP_CLOSE;
	sock->state = SS_UNCONNECTED;

	kfree(ax25->digipeat);
	ax25->digipeat = NULL;

	/*
	 *	Handle digi-peaters to be used.
	 */
	if (addr_len > sizeof(struct sockaddr_ax25) &&
	    fsa->fsa_ax25.sax25_ndigis != 0) {
		/* Valid number of digipeaters ? */
		if (fsa->fsa_ax25.sax25_ndigis < 1 || fsa->fsa_ax25.sax25_ndigis > AX25_MAX_DIGIS) {
			err = -EINVAL;
			goto out_release;
		}

		if ((digi = kmalloc(sizeof(ax25_digi), GFP_KERNEL)) == NULL) {
			err = -ENOBUFS;
			goto out_release;
		}

		digi->ndigi      = fsa->fsa_ax25.sax25_ndigis;
		digi->lastrepeat = -1;

		while (ct < fsa->fsa_ax25.sax25_ndigis) {
			if ((fsa->fsa_digipeater[ct].ax25_call[6] &
			     AX25_HBIT) && ax25->iamdigi) {
				digi->repeated[ct] = 1;
				digi->lastrepeat   = ct;
			} else {
				digi->repeated[ct] = 0;
			}
			digi->calls[ct] = fsa->fsa_digipeater[ct];
			ct++;
		}
	}

	/*
	 *	Must bind first - autobinding in this may or may not work. If
	 *	the socket is already bound, check to see if the device has
	 *	been filled in, error if it hasn't.
	 */
	if (sock_flag(sk, SOCK_ZAPPED)) {
		/* check if we can remove this feature. It is broken. */
		printk(KERN_WARNING "ax25_connect(): %s uses autobind, please contact jreuter@yaina.de\n",
			current->comm);
		if ((err = ax25_rt_autobind(ax25, &fsa->fsa_ax25.sax25_call)) < 0) {
			kfree(digi);
			goto out_release;
		}

		ax25_fillin_cb(ax25, ax25->ax25_dev);
		ax25_cb_add(ax25);
	} else {
		if (ax25->ax25_dev == NULL) {
			kfree(digi);
			err = -EHOSTUNREACH;
			goto out_release;
		}
	}

	if (sk->sk_type == SOCK_SEQPACKET &&
	    (ax25t=ax25_find_cb(&ax25->source_addr, &fsa->fsa_ax25.sax25_call, digi,
			 ax25->ax25_dev->dev))) {
		kfree(digi);
		err = -EADDRINUSE;		/* Already such a connection */
		ax25_cb_put(ax25t);
		goto out_release;
	}

	ax25->dest_addr = fsa->fsa_ax25.sax25_call;
	ax25->digipeat  = digi;

	/* First the easy one */
	if (sk->sk_type != SOCK_SEQPACKET) {
		sock->state = SS_CONNECTED;
		sk->sk_state   = TCP_ESTABLISHED;
		goto out_release;
	}

	/* Move to connecting socket, ax.25 lapb WAIT_UA.. */
	sock->state        = SS_CONNECTING;
	sk->sk_state          = TCP_SYN_SENT;

	switch (ax25->ax25_dev->values[AX25_VALUES_PROTOCOL]) {
	case AX25_PROTO_STD_SIMPLEX:
	case AX25_PROTO_STD_DUPLEX:
		ax25_std_establish_data_link(ax25);
		break;

#ifdef CONFIG_AX25_DAMA_SLAVE
	case AX25_PROTO_DAMA_SLAVE:
		ax25->modulus = AX25_MODULUS;
		ax25->window  = ax25->ax25_dev->values[AX25_VALUES_WINDOW];
		if (ax25->ax25_dev->dama.slave)
			ax25_ds_establish_data_link(ax25);
		else
			ax25_std_establish_data_link(ax25);
		break;
#endif
	}

	ax25->state = AX25_STATE_1;

	ax25_start_heartbeat(ax25);

	/* Now the loop */
	if (sk->sk_state != TCP_ESTABLISHED && (flags & O_NONBLOCK)) {
		err = -EINPROGRESS;
		goto out_release;
	}

	if (sk->sk_state == TCP_SYN_SENT) {
		DEFINE_WAIT(wait);

		for (;;) {
			prepare_to_wait(sk_sleep(sk), &wait,
					TASK_INTERRUPTIBLE);
			if (sk->sk_state != TCP_SYN_SENT)
				break;
			if (!signal_pending(current)) {
				release_sock(sk);
				schedule();
				lock_sock(sk);
				continue;
			}
			err = -ERESTARTSYS;
			break;
		}
		finish_wait(sk_sleep(sk), &wait);

		if (err)
			goto out_release;
	}

	if (sk->sk_state != TCP_ESTABLISHED) {
		/* Not in ABM, not in WAIT_UA -> failed */
		sock->state = SS_UNCONNECTED;
		err = sock_error(sk);	/* Always set at this point */
		goto out_release;
	}

	sock->state = SS_CONNECTED;

	err = 0;
out_release:
	release_sock(sk);

	return err;
}
