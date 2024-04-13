static int afiucv_netdev_event(struct notifier_block *this,
			       unsigned long event, void *ptr)
{
	struct net_device *event_dev = netdev_notifier_info_to_dev(ptr);
	struct sock *sk;
	struct iucv_sock *iucv;

	switch (event) {
	case NETDEV_REBOOT:
	case NETDEV_GOING_DOWN:
		sk_for_each(sk, &iucv_sk_list.head) {
			iucv = iucv_sk(sk);
			if ((iucv->hs_dev == event_dev) &&
			    (sk->sk_state == IUCV_CONNECTED)) {
				if (event == NETDEV_GOING_DOWN)
					iucv_send_ctrl(sk, AF_IUCV_FLAG_FIN);
				sk->sk_state = IUCV_DISCONN;
				sk->sk_state_change(sk);
			}
		}
		break;
	case NETDEV_DOWN:
	case NETDEV_UNREGISTER:
	default:
		break;
	}
	return NOTIFY_DONE;
}
