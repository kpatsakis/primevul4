static void __iriap_close(struct iriap_cb *self)
{
	IRDA_DEBUG(4, "%s()\n", __func__);

	IRDA_ASSERT(self != NULL, return;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return;);

	del_timer(&self->watchdog_timer);

	if (self->request_skb)
		dev_kfree_skb(self->request_skb);

	self->magic = 0;

	kfree(self);
}
