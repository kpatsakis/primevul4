static s32 brcmf_configure_opensecurity(struct brcmf_if *ifp)
{
	s32 err;

	/* set auth */
	err = brcmf_fil_bsscfg_int_set(ifp, "auth", 0);
	if (err < 0) {
		brcmf_err("auth error %d\n", err);
		return err;
	}
	/* set wsec */
	err = brcmf_fil_bsscfg_int_set(ifp, "wsec", 0);
	if (err < 0) {
		brcmf_err("wsec error %d\n", err);
		return err;
	}
	/* set upper-layer auth */
	err = brcmf_fil_bsscfg_int_set(ifp, "wpa_auth", WPA_AUTH_NONE);
	if (err < 0) {
		brcmf_err("wpa_auth error %d\n", err);
		return err;
	}

	return 0;
}
