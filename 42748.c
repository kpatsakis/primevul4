struct iriap_cb *iriap_open(__u8 slsap_sel, int mode, void *priv,
			    CONFIRM_CALLBACK callback)
{
	struct iriap_cb *self;

	IRDA_DEBUG(2, "%s()\n", __func__);

	self = kzalloc(sizeof(*self), GFP_ATOMIC);
	if (!self) {
		IRDA_WARNING("%s: Unable to kmalloc!\n", __func__);
		return NULL;
	}

	/*
	 *  Initialize instance
	 */

	self->magic = IAS_MAGIC;
	self->mode = mode;
	if (mode == IAS_CLIENT)
		iriap_register_lsap(self, slsap_sel, mode);

	self->confirm = callback;
	self->priv = priv;

	/* iriap_getvaluebyclass_request() will construct packets before
	 * we connect, so this must have a sane value... Jean II */
	self->max_header_size = LMP_MAX_HEADER;

	init_timer(&self->watchdog_timer);

	hashbin_insert(iriap, (irda_queue_t *) self, (long) self, NULL);

	/* Initialize state machines */
	iriap_next_client_state(self, S_DISCONNECT);
	iriap_next_call_state(self, S_MAKE_CALL);
	iriap_next_server_state(self, R_DISCONNECT);
	iriap_next_r_connect_state(self, R_WAITING);

	return self;
}
