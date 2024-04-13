static void iriap_getvaluebyclass_response(struct iriap_cb *self,
					   __u16 obj_id,
					   __u8 ret_code,
					   struct ias_value *value)
{
	struct sk_buff *tx_skb;
	int n;
	__be32 tmp_be32;
	__be16 tmp_be16;
	__u8 *fp;

	IRDA_DEBUG(4, "%s()\n", __func__);

	IRDA_ASSERT(self != NULL, return;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return;);
	IRDA_ASSERT(value != NULL, return;);
	IRDA_ASSERT(value->len <= 1024, return;);

	/* Initialize variables */
	n = 0;

	/*
	 *  We must adjust the size of the response after the length of the
	 *  value. We add 32 bytes because of the 6 bytes for the frame and
	 *  max 5 bytes for the value coding.
	 */
	tx_skb = alloc_skb(value->len + self->max_header_size + 32,
			   GFP_ATOMIC);
	if (!tx_skb)
		return;

	/* Reserve space for MUX and LAP header */
	skb_reserve(tx_skb, self->max_header_size);
	skb_put(tx_skb, 6);

	fp = tx_skb->data;

	/* Build frame */
	fp[n++] = GET_VALUE_BY_CLASS | IAP_LST;
	fp[n++] = ret_code;

	/* Insert list length (MSB first) */
	tmp_be16 = htons(0x0001);
	memcpy(fp+n, &tmp_be16, 2);  n += 2;

	/* Insert object identifier ( MSB first) */
	tmp_be16 = cpu_to_be16(obj_id);
	memcpy(fp+n, &tmp_be16, 2); n += 2;

	switch (value->type) {
	case IAS_STRING:
		skb_put(tx_skb, 3 + value->len);
		fp[n++] = value->type;
		fp[n++] = 0; /* ASCII */
		fp[n++] = (__u8) value->len;
		memcpy(fp+n, value->t.string, value->len); n+=value->len;
		break;
	case IAS_INTEGER:
		skb_put(tx_skb, 5);
		fp[n++] = value->type;

		tmp_be32 = cpu_to_be32(value->t.integer);
		memcpy(fp+n, &tmp_be32, 4); n += 4;
		break;
	case IAS_OCT_SEQ:
		skb_put(tx_skb, 3 + value->len);
		fp[n++] = value->type;

		tmp_be16 = cpu_to_be16(value->len);
		memcpy(fp+n, &tmp_be16, 2); n += 2;
		memcpy(fp+n, value->t.oct_seq, value->len); n+=value->len;
		break;
	case IAS_MISSING:
		IRDA_DEBUG( 3, "%s: sending IAS_MISSING\n", __func__);
		skb_put(tx_skb, 1);
		fp[n++] = value->type;
		break;
	default:
		IRDA_DEBUG(0, "%s(), type not implemented!\n", __func__);
		break;
	}
	iriap_do_r_connect_event(self, IAP_CALL_RESPONSE, tx_skb);

	/* Drop reference count - see state_r_execute(). */
	dev_kfree_skb(tx_skb);
}
