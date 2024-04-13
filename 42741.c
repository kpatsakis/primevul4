void iriap_connect_request(struct iriap_cb *self)
{
	int ret;

	IRDA_ASSERT(self != NULL, return;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return;);

	ret = irlmp_connect_request(self->lsap, LSAP_IAS,
				    self->saddr, self->daddr,
				    NULL, NULL);
	if (ret < 0) {
		IRDA_DEBUG(0, "%s(), connect failed!\n", __func__);
		self->confirm(IAS_DISCONNECT, 0, NULL, self->priv);
	}
}
