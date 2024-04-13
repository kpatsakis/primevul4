static int hci_sock_sendmsg(struct kiocb *iocb, struct socket *sock,
			    struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct hci_dev *hdev;
	struct sk_buff *skb;
	int err;

	BT_DBG("sock %p sk %p", sock, sk);

	if (msg->msg_flags & MSG_OOB)
		return -EOPNOTSUPP;

	if (msg->msg_flags & ~(MSG_DONTWAIT|MSG_NOSIGNAL|MSG_ERRQUEUE))
		return -EINVAL;

	if (len < 4 || len > HCI_MAX_FRAME_SIZE)
		return -EINVAL;

	lock_sock(sk);

	switch (hci_pi(sk)->channel) {
	case HCI_CHANNEL_RAW:
	case HCI_CHANNEL_USER:
		break;
	case HCI_CHANNEL_CONTROL:
		err = mgmt_control(sk, msg, len);
		goto done;
	case HCI_CHANNEL_MONITOR:
		err = -EOPNOTSUPP;
		goto done;
	default:
		err = -EINVAL;
		goto done;
	}

	hdev = hci_pi(sk)->hdev;
	if (!hdev) {
		err = -EBADFD;
		goto done;
	}

	if (!test_bit(HCI_UP, &hdev->flags)) {
		err = -ENETDOWN;
		goto done;
	}

	skb = bt_skb_send_alloc(sk, len, msg->msg_flags & MSG_DONTWAIT, &err);
	if (!skb)
		goto done;

	if (memcpy_fromiovec(skb_put(skb, len), msg->msg_iov, len)) {
		err = -EFAULT;
		goto drop;
	}

	bt_cb(skb)->pkt_type = *((unsigned char *) skb->data);
	skb_pull(skb, 1);

	if (hci_pi(sk)->channel == HCI_CHANNEL_RAW &&
	    bt_cb(skb)->pkt_type == HCI_COMMAND_PKT) {
		u16 opcode = get_unaligned_le16(skb->data);
		u16 ogf = hci_opcode_ogf(opcode);
		u16 ocf = hci_opcode_ocf(opcode);

		if (((ogf > HCI_SFLT_MAX_OGF) ||
		     !hci_test_bit(ocf & HCI_FLT_OCF_BITS,
				   &hci_sec_filter.ocf_mask[ogf])) &&
		    !capable(CAP_NET_RAW)) {
			err = -EPERM;
			goto drop;
		}

		if (test_bit(HCI_RAW, &hdev->flags) || (ogf == 0x3f)) {
			skb_queue_tail(&hdev->raw_q, skb);
			queue_work(hdev->workqueue, &hdev->tx_work);
		} else {
			/* Stand-alone HCI commands must be flaged as
			 * single-command requests.
			 */
			bt_cb(skb)->req.start = true;

			skb_queue_tail(&hdev->cmd_q, skb);
			queue_work(hdev->workqueue, &hdev->cmd_work);
		}
	} else {
		if (!capable(CAP_NET_RAW)) {
			err = -EPERM;
			goto drop;
		}

		if (hci_pi(sk)->channel == HCI_CHANNEL_USER &&
		    bt_cb(skb)->pkt_type != HCI_COMMAND_PKT &&
		    bt_cb(skb)->pkt_type != HCI_ACLDATA_PKT &&
		    bt_cb(skb)->pkt_type != HCI_SCODATA_PKT) {
			err = -EINVAL;
			goto drop;
		}

		skb_queue_tail(&hdev->raw_q, skb);
		queue_work(hdev->workqueue, &hdev->tx_work);
	}

	err = len;

done:
	release_sock(sk);
	return err;

drop:
	kfree_skb(skb);
	goto done;
}
