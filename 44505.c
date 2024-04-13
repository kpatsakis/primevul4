dccp_timeout_obj_to_nlattr(struct sk_buff *skb, const void *data)
{
        const unsigned int *timeouts = data;
	int i;

	for (i=CTA_TIMEOUT_DCCP_UNSPEC+1; i<CTA_TIMEOUT_DCCP_MAX+1; i++) {
		if (nla_put_be32(skb, i, htonl(timeouts[i] / HZ)))
			goto nla_put_failure;
	}
	return 0;

nla_put_failure:
	return -ENOSPC;
}
