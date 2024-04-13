brcmf_get_netinfo_array(struct brcmf_pno_scanresults_le *pfn_v1)
{
	struct brcmf_pno_scanresults_v2_le *pfn_v2;
	struct brcmf_pno_net_info_le *netinfo;

	switch (pfn_v1->version) {
	default:
		WARN_ON(1);
		/* fall-thru */
	case cpu_to_le32(1):
		netinfo = (struct brcmf_pno_net_info_le *)(pfn_v1 + 1);
		break;
	case cpu_to_le32(2):
		pfn_v2 = (struct brcmf_pno_scanresults_v2_le *)pfn_v1;
		netinfo = (struct brcmf_pno_net_info_le *)(pfn_v2 + 1);
		break;
	}

	return netinfo;
}
