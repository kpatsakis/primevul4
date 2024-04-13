ppp_receive_error(struct ppp *ppp)
{
	++ppp->dev->stats.rx_errors;
	if (ppp->vj)
		slhc_toss(ppp->vj);
}
