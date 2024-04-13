void iriap_call_indication(struct iriap_cb *self, struct sk_buff *skb)
{
	__u8 *fp;
	__u8 opcode;

	IRDA_DEBUG(4, "%s()\n", __func__);

	IRDA_ASSERT(self != NULL, return;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return;);
	IRDA_ASSERT(skb != NULL, return;);

	fp = skb->data;

	opcode = fp[0];
	if (~opcode & 0x80) {
		IRDA_WARNING("%s: IrIAS multiframe commands or results "
			     "is not implemented yet!\n", __func__);
		return;
	}
	opcode &= 0x7f; /* Mask away LST bit */

	switch (opcode) {
	case GET_INFO_BASE:
		IRDA_WARNING("%s: GetInfoBaseDetails not implemented yet!\n",
			     __func__);
		break;
	case GET_VALUE_BY_CLASS:
		iriap_getvaluebyclass_indication(self, skb);
		break;
	}
	/* skb will be cleaned up in iriap_data_indication */
}
