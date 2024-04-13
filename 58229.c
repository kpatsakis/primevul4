static int cxusb_read_status(struct dvb_frontend *fe,
				  enum fe_status *status)
{
	struct dvb_usb_adapter *adap = (struct dvb_usb_adapter *)fe->dvb->priv;
	struct cxusb_state *state = (struct cxusb_state *)adap->dev->priv;
	int ret;

	ret = state->fe_read_status(fe, status);

	/* it need resync slave fifo when signal change from unlock to lock.*/
	if ((*status & FE_HAS_LOCK) && (!state->last_lock)) {
		mutex_lock(&state->stream_mutex);
		cxusb_streaming_ctrl(adap, 1);
		mutex_unlock(&state->stream_mutex);
	}

	state->last_lock = (*status & FE_HAS_LOCK) ? 1 : 0;
	return ret;
}
