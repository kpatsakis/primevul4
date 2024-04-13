static void gs_free_tx_context(struct gs_tx_context *txc)
{
	txc->echo_id = GS_MAX_TX_URBS;
}
