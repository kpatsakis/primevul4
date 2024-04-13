static int hci_sock_setsockopt(struct socket *sock, int level, int optname,
			       char __user *optval, unsigned int len)
{
	struct hci_ufilter uf = { .opcode = 0 };
	struct sock *sk = sock->sk;
	int err = 0, opt = 0;

	BT_DBG("sk %p, opt %d", sk, optname);

	lock_sock(sk);

	if (hci_pi(sk)->channel != HCI_CHANNEL_RAW) {
		err = -EINVAL;
		goto done;
	}

	switch (optname) {
	case HCI_DATA_DIR:
		if (get_user(opt, (int __user *)optval)) {
			err = -EFAULT;
			break;
		}

		if (opt)
			hci_pi(sk)->cmsg_mask |= HCI_CMSG_DIR;
		else
			hci_pi(sk)->cmsg_mask &= ~HCI_CMSG_DIR;
		break;

	case HCI_TIME_STAMP:
		if (get_user(opt, (int __user *)optval)) {
			err = -EFAULT;
			break;
		}

		if (opt)
			hci_pi(sk)->cmsg_mask |= HCI_CMSG_TSTAMP;
		else
			hci_pi(sk)->cmsg_mask &= ~HCI_CMSG_TSTAMP;
		break;

	case HCI_FILTER:
		{
			struct hci_filter *f = &hci_pi(sk)->filter;

			uf.type_mask = f->type_mask;
			uf.opcode    = f->opcode;
			uf.event_mask[0] = *((u32 *) f->event_mask + 0);
			uf.event_mask[1] = *((u32 *) f->event_mask + 1);
		}

		len = min_t(unsigned int, len, sizeof(uf));
		if (copy_from_user(&uf, optval, len)) {
			err = -EFAULT;
			break;
		}

		if (!capable(CAP_NET_RAW)) {
			uf.type_mask &= hci_sec_filter.type_mask;
			uf.event_mask[0] &= *((u32 *) hci_sec_filter.event_mask + 0);
			uf.event_mask[1] &= *((u32 *) hci_sec_filter.event_mask + 1);
		}

		{
			struct hci_filter *f = &hci_pi(sk)->filter;

			f->type_mask = uf.type_mask;
			f->opcode    = uf.opcode;
			*((u32 *) f->event_mask + 0) = uf.event_mask[0];
			*((u32 *) f->event_mask + 1) = uf.event_mask[1];
		}
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

done:
	release_sock(sk);
	return err;
}
