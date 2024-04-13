static int packet_rcv_fanout(struct sk_buff *skb, struct net_device *dev,
			     struct packet_type *pt, struct net_device *orig_dev)
{
	struct packet_fanout *f = pt->af_packet_priv;
	unsigned int num = READ_ONCE(f->num_members);
	struct net *net = read_pnet(&f->net);
	struct packet_sock *po;
	unsigned int idx;

	if (!net_eq(dev_net(dev), net) || !num) {
		kfree_skb(skb);
		return 0;
	}

	if (fanout_has_flag(f, PACKET_FANOUT_FLAG_DEFRAG)) {
		skb = ip_check_defrag(net, skb, IP_DEFRAG_AF_PACKET);
		if (!skb)
			return 0;
	}
	switch (f->type) {
	case PACKET_FANOUT_HASH:
	default:
		idx = fanout_demux_hash(f, skb, num);
		break;
	case PACKET_FANOUT_LB:
		idx = fanout_demux_lb(f, skb, num);
		break;
	case PACKET_FANOUT_CPU:
		idx = fanout_demux_cpu(f, skb, num);
		break;
	case PACKET_FANOUT_RND:
		idx = fanout_demux_rnd(f, skb, num);
		break;
	case PACKET_FANOUT_QM:
		idx = fanout_demux_qm(f, skb, num);
		break;
	case PACKET_FANOUT_ROLLOVER:
		idx = fanout_demux_rollover(f, skb, 0, false, num);
		break;
	case PACKET_FANOUT_CBPF:
	case PACKET_FANOUT_EBPF:
		idx = fanout_demux_bpf(f, skb, num);
		break;
	}

	if (fanout_has_flag(f, PACKET_FANOUT_FLAG_ROLLOVER))
		idx = fanout_demux_rollover(f, skb, idx, true, num);

	po = pkt_sk(f->arr[idx]);
	return po->prot_hook.func(skb, dev, &po->prot_hook, orig_dev);
}
