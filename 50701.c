static void prb_fill_vlan_info(struct tpacket_kbdq_core *pkc,
			struct tpacket3_hdr *ppd)
{
	if (skb_vlan_tag_present(pkc->skb)) {
		ppd->hv1.tp_vlan_tci = skb_vlan_tag_get(pkc->skb);
		ppd->hv1.tp_vlan_tpid = ntohs(pkc->skb->vlan_proto);
		ppd->tp_status = TP_STATUS_VLAN_VALID | TP_STATUS_VLAN_TPID_VALID;
	} else {
		ppd->hv1.tp_vlan_tci = 0;
		ppd->hv1.tp_vlan_tpid = 0;
		ppd->tp_status = TP_STATUS_AVAILABLE;
	}
}
