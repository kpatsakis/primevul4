brcmf_configure_arp_nd_offload(struct brcmf_if *ifp, bool enable)
{
	s32 err;
	u32 mode;

	if (enable)
		mode = BRCMF_ARP_OL_AGENT | BRCMF_ARP_OL_PEER_AUTO_REPLY;
	else
		mode = 0;

	/* Try to set and enable ARP offload feature, this may fail, then it  */
	/* is simply not supported and err 0 will be returned                 */
	err = brcmf_fil_iovar_int_set(ifp, "arp_ol", mode);
	if (err) {
		brcmf_dbg(TRACE, "failed to set ARP offload mode to 0x%x, err = %d\n",
			  mode, err);
		err = 0;
	} else {
		err = brcmf_fil_iovar_int_set(ifp, "arpoe", enable);
		if (err) {
			brcmf_dbg(TRACE, "failed to configure (%d) ARP offload err = %d\n",
				  enable, err);
			err = 0;
		} else
			brcmf_dbg(TRACE, "successfully configured (%d) ARP offload to 0x%x\n",
				  enable, mode);
	}

	err = brcmf_fil_iovar_int_set(ifp, "ndoe", enable);
	if (err) {
		brcmf_dbg(TRACE, "failed to configure (%d) ND offload err = %d\n",
			  enable, err);
		err = 0;
	} else
		brcmf_dbg(TRACE, "successfully configured (%d) ND offload to 0x%x\n",
			  enable, mode);

	return err;
}
