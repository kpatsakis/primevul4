static void prb_fill_rxhash(struct tpacket_kbdq_core *pkc,
			struct tpacket3_hdr *ppd)
{
	ppd->hv1.tp_rxhash = skb_get_hash(pkc->skb);
}
