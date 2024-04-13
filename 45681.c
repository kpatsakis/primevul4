static void prb_clear_rxhash(struct tpacket_kbdq_core *pkc,
			struct tpacket3_hdr *ppd)
{
	ppd->hv1.tp_rxhash = 0;
}
