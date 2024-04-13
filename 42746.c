int iriap_getvaluebyclass_request(struct iriap_cb *self,
				  __u32 saddr, __u32 daddr,
				  char *name, char *attr)
{
	struct sk_buff *tx_skb;
	int name_len, attr_len, skb_len;
	__u8 *frame;

	IRDA_ASSERT(self != NULL, return -1;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return -1;);

	/* Client must supply the destination device address */
	if (!daddr)
		return -1;

	self->daddr = daddr;
	self->saddr = saddr;

	/*
	 *  Save operation, so we know what the later indication is about
	 */
	self->operation = GET_VALUE_BY_CLASS;

	/* Give ourselves 10 secs to finish this operation */
	iriap_start_watchdog_timer(self, 10*HZ);

	name_len = strlen(name);	/* Up to IAS_MAX_CLASSNAME = 60 */
	attr_len = strlen(attr);	/* Up to IAS_MAX_ATTRIBNAME = 60 */

	skb_len = self->max_header_size+2+name_len+1+attr_len+4;
	tx_skb = alloc_skb(skb_len, GFP_ATOMIC);
	if (!tx_skb)
		return -ENOMEM;

	/* Reserve space for MUX and LAP header */
	skb_reserve(tx_skb, self->max_header_size);
	skb_put(tx_skb, 3+name_len+attr_len);
	frame = tx_skb->data;

	/* Build frame */
	frame[0] = IAP_LST | GET_VALUE_BY_CLASS;
	frame[1] = name_len;                       /* Insert length of name */
	memcpy(frame+2, name, name_len);           /* Insert name */
	frame[2+name_len] = attr_len;              /* Insert length of attr */
	memcpy(frame+3+name_len, attr, attr_len);  /* Insert attr */

	iriap_do_client_event(self, IAP_CALL_REQUEST_GVBC, tx_skb);

	/* Drop reference count - see state_s_disconnect(). */
	dev_kfree_skb(tx_skb);

	return 0;
}
