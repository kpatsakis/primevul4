void iriap_close(struct iriap_cb *self)
{
	struct iriap_cb *entry;

	IRDA_DEBUG(2, "%s()\n", __func__);

	IRDA_ASSERT(self != NULL, return;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return;);

	if (self->lsap) {
		irlmp_close_lsap(self->lsap);
		self->lsap = NULL;
	}

	entry = (struct iriap_cb *) hashbin_remove(iriap, (long) self, NULL);
	IRDA_ASSERT(entry == self, return;);

	__iriap_close(self);
}
