static int br_nf_dev_queue_xmit(struct sk_buff *skb)
{
        return br_dev_queue_push_xmit(skb);
}
