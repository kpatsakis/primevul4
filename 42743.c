static void iriap_disconnect_indication(void *instance, void *sap,
					LM_REASON reason,
					struct sk_buff *skb)
{
	struct iriap_cb *self;

	IRDA_DEBUG(4, "%s(), reason=%s\n", __func__, irlmp_reasons[reason]);

	self = (struct iriap_cb *) instance;

	IRDA_ASSERT(self != NULL, return;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return;);

	IRDA_ASSERT(iriap != NULL, return;);

	del_timer(&self->watchdog_timer);

	/* Not needed */
	if (skb)
		dev_kfree_skb(skb);

	if (self->mode == IAS_CLIENT) {
		IRDA_DEBUG(4, "%s(), disconnect as client\n", __func__);


		iriap_do_client_event(self, IAP_LM_DISCONNECT_INDICATION,
				      NULL);
		/*
		 * Inform service user that the request failed by sending
		 * it a NULL value. Warning, the client might close us, so
		 * remember no to use self anymore after calling confirm
		 */
		if (self->confirm)
			self->confirm(IAS_DISCONNECT, 0, NULL, self->priv);
	} else {
		IRDA_DEBUG(4, "%s(), disconnect as server\n", __func__);
		iriap_do_server_event(self, IAP_LM_DISCONNECT_INDICATION,
				      NULL);
		iriap_close(self);
	}
}
