static s32 brcmf_set_frag(struct net_device *ndev, u32 frag_threshold)
{
	s32 err = 0;

	err = brcmf_fil_iovar_int_set(netdev_priv(ndev), "fragthresh",
				      frag_threshold);
	if (err)
		brcmf_err("Error (%d)\n", err);

	return err;
}
