static int iriap_register_lsap(struct iriap_cb *self, __u8 slsap_sel, int mode)
{
	notify_t notify;

	IRDA_DEBUG(2, "%s()\n", __func__);

	irda_notify_init(&notify);
	notify.connect_confirm       = iriap_connect_confirm;
	notify.connect_indication    = iriap_connect_indication;
	notify.disconnect_indication = iriap_disconnect_indication;
	notify.data_indication       = iriap_data_indication;
	notify.instance = self;
	if (mode == IAS_CLIENT)
		strcpy(notify.name, "IrIAS cli");
	else
		strcpy(notify.name, "IrIAS srv");

	self->lsap = irlmp_open_lsap(slsap_sel, &notify, 0);
	if (self->lsap == NULL) {
		IRDA_ERROR("%s: Unable to allocated LSAP!\n", __func__);
		return -1;
	}
	self->slsap_sel = self->lsap->slsap_sel;

	return 0;
}
