static void iriap_connect_confirm(void *instance, void *sap,
				  struct qos_info *qos, __u32 max_seg_size,
				  __u8 max_header_size,
				  struct sk_buff *skb)
{
	struct iriap_cb *self;

	self = (struct iriap_cb *) instance;

	IRDA_ASSERT(self != NULL, return;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return;);
	IRDA_ASSERT(skb != NULL, return;);

	self->max_data_size = max_seg_size;
	self->max_header_size = max_header_size;

	del_timer(&self->watchdog_timer);

	iriap_do_client_event(self, IAP_LM_CONNECT_CONFIRM, skb);

	/* Drop reference count - see state_s_make_call(). */
	dev_kfree_skb(skb);
}
