static bool ath_tid_has_buffered(struct ath_atx_tid *tid)
{
	return !skb_queue_empty(&tid->buf_q) || !skb_queue_empty(&tid->retry_q);
}
