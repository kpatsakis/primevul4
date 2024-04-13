send_key_to_dongle(struct brcmf_if *ifp, struct brcmf_wsec_key *key)
{
	int err;
	struct brcmf_wsec_key_le key_le;

	convert_key_from_CPU(key, &key_le);

	brcmf_netdev_wait_pend8021x(ifp);

	err = brcmf_fil_bsscfg_data_set(ifp, "wsec_key", &key_le,
					sizeof(key_le));

	if (err)
		brcmf_err("wsec_key error (%d)\n", err);
	return err;
}
