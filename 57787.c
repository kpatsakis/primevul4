static void dccp_v6_reqsk_destructor(struct request_sock *req)
{
	dccp_feat_list_purge(&dccp_rsk(req)->dreq_featneg);
	kfree(inet_rsk(req)->ipv6_opt);
	kfree_skb(inet_rsk(req)->pktopts);
}
