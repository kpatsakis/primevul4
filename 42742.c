static int iriap_data_indication(void *instance, void *sap,
				 struct sk_buff *skb)
{
	struct iriap_cb *self;
	__u8  *frame;
	__u8  opcode;

	IRDA_DEBUG(3, "%s()\n", __func__);

	self = (struct iriap_cb *) instance;

	IRDA_ASSERT(skb != NULL, return 0;);
	IRDA_ASSERT(self != NULL, goto out;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, goto out;);

	frame = skb->data;

	if (self->mode == IAS_SERVER) {
		/* Call server */
		IRDA_DEBUG(4, "%s(), Calling server!\n", __func__);
		iriap_do_r_connect_event(self, IAP_RECV_F_LST, skb);
		goto out;
	}
	opcode = frame[0];
	if (~opcode & IAP_LST) {
		IRDA_WARNING("%s:, IrIAS multiframe commands or "
			     "results is not implemented yet!\n",
			     __func__);
		goto out;
	}

	/* Check for ack frames since they don't contain any data */
	if (opcode & IAP_ACK) {
		IRDA_DEBUG(0, "%s() Got ack frame!\n", __func__);
		goto out;
	}

	opcode &= ~IAP_LST; /* Mask away LST bit */

	switch (opcode) {
	case GET_INFO_BASE:
		IRDA_DEBUG(0, "IrLMP GetInfoBaseDetails not implemented!\n");
		break;
	case GET_VALUE_BY_CLASS:
		iriap_do_call_event(self, IAP_RECV_F_LST, NULL);

		switch (frame[1]) {
		case IAS_SUCCESS:
			iriap_getvaluebyclass_confirm(self, skb);
			break;
		case IAS_CLASS_UNKNOWN:
			IRDA_DEBUG(1, "%s(), No such class!\n", __func__);
			/* Finished, close connection! */
			iriap_disconnect_request(self);

			/*
			 * Warning, the client might close us, so remember
			 * no to use self anymore after calling confirm
			 */
			if (self->confirm)
				self->confirm(IAS_CLASS_UNKNOWN, 0, NULL,
					      self->priv);
			break;
		case IAS_ATTRIB_UNKNOWN:
			IRDA_DEBUG(1, "%s(), No such attribute!\n", __func__);
			/* Finished, close connection! */
			iriap_disconnect_request(self);

			/*
			 * Warning, the client might close us, so remember
			 * no to use self anymore after calling confirm
			 */
			if (self->confirm)
				self->confirm(IAS_ATTRIB_UNKNOWN, 0, NULL,
					      self->priv);
			break;
		}
		break;
	default:
		IRDA_DEBUG(0, "%s(), Unknown op-code: %02x\n", __func__,
			   opcode);
		break;
	}

out:
	/* Cleanup - sub-calls will have done skb_get() as needed. */
	dev_kfree_skb(skb);
	return 0;
}
