static void iriap_connect_indication(void *instance, void *sap,
				     struct qos_info *qos, __u32 max_seg_size,
				     __u8 max_header_size,
				     struct sk_buff *skb)
{
	struct iriap_cb *self, *new;

	IRDA_DEBUG(1, "%s()\n", __func__);

	self = (struct iriap_cb *) instance;

	IRDA_ASSERT(skb != NULL, return;);
	IRDA_ASSERT(self != NULL, goto out;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, goto out;);

	/* Start new server */
	new = iriap_open(LSAP_IAS, IAS_SERVER, NULL, NULL);
	if (!new) {
		IRDA_DEBUG(0, "%s(), open failed\n", __func__);
		goto out;
	}

	/* Now attach up the new "socket" */
	new->lsap = irlmp_dup(self->lsap, new);
	if (!new->lsap) {
		IRDA_DEBUG(0, "%s(), dup failed!\n", __func__);
		goto out;
	}

	new->max_data_size = max_seg_size;
	new->max_header_size = max_header_size;

	/* Clean up the original one to keep it in listen state */
	irlmp_listen(self->lsap);

	iriap_do_server_event(new, IAP_LM_CONNECT_INDICATION, skb);

out:
	/* Drop reference count - see state_r_disconnect(). */
	dev_kfree_skb(skb);
}
