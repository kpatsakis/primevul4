static void iriap_watchdog_timer_expired(void *data)
{
	struct iriap_cb *self = (struct iriap_cb *) data;

	IRDA_ASSERT(self != NULL, return;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return;);

	/* iriap_close(self); */
}
