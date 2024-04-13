static int xfrm_notify_policy_flush(const struct km_event *c)
{
	struct net *net = c->net;
	struct nlmsghdr *nlh;
	struct sk_buff *skb;
	int err;

	skb = nlmsg_new(userpolicy_type_attrsize(), GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	nlh = nlmsg_put(skb, c->pid, c->seq, XFRM_MSG_FLUSHPOLICY, 0, 0);
	err = -EMSGSIZE;
	if (nlh == NULL)
		goto out_free_skb;
	err = copy_to_user_policy_type(c->data.type, skb);
	if (err)
		goto out_free_skb;

	nlmsg_end(skb, nlh);

	return nlmsg_multicast(net->xfrm.nlsk, skb, 0, XFRMNLGRP_POLICY, GFP_ATOMIC);

out_free_skb:
	kfree_skb(skb);
	return err;
}
