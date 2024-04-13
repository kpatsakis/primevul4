static int __init af_unix_init(void)
{
	int rc = -1;

	BUILD_BUG_ON(sizeof(struct unix_skb_parms) > FIELD_SIZEOF(struct sk_buff, cb));

	rc = proto_register(&unix_proto, 1);
	if (rc != 0) {
		printk(KERN_CRIT "%s: Cannot create unix_sock SLAB cache!\n",
		       __func__);
		goto out;
	}

	sock_register(&unix_family_ops);
	register_pernet_subsys(&unix_net_ops);
out:
	return rc;
}
