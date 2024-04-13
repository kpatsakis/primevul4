static int rtnl_link_fill(struct sk_buff *skb, const struct net_device *dev)
{
	const struct rtnl_link_ops *ops = dev->rtnl_link_ops;
	struct nlattr *linkinfo, *data;
	int err = -EMSGSIZE;

	linkinfo = nla_nest_start(skb, IFLA_LINKINFO);
	if (linkinfo == NULL)
		goto out;

	if (nla_put_string(skb, IFLA_INFO_KIND, ops->kind) < 0)
		goto err_cancel_link;
	if (ops->fill_xstats) {
		err = ops->fill_xstats(skb, dev);
		if (err < 0)
			goto err_cancel_link;
	}
	if (ops->fill_info) {
		data = nla_nest_start(skb, IFLA_INFO_DATA);
		if (data == NULL)
			goto err_cancel_link;
		err = ops->fill_info(skb, dev);
		if (err < 0)
			goto err_cancel_data;
		nla_nest_end(skb, data);
	}

	nla_nest_end(skb, linkinfo);
	return 0;

err_cancel_data:
	nla_nest_cancel(skb, data);
err_cancel_link:
	nla_nest_cancel(skb, linkinfo);
out:
	return err;
}
