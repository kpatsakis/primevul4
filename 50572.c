static s32 brcmf_dongle_roam(struct brcmf_if *ifp)
{
	s32 err;
	u32 bcn_timeout;
	__le32 roamtrigger[2];
	__le32 roam_delta[2];

	/* Configure beacon timeout value based upon roaming setting */
	if (ifp->drvr->settings->roamoff)
		bcn_timeout = BRCMF_DEFAULT_BCN_TIMEOUT_ROAM_OFF;
	else
		bcn_timeout = BRCMF_DEFAULT_BCN_TIMEOUT_ROAM_ON;
	err = brcmf_fil_iovar_int_set(ifp, "bcn_timeout", bcn_timeout);
	if (err) {
		brcmf_err("bcn_timeout error (%d)\n", err);
		goto roam_setup_done;
	}

	/* Enable/Disable built-in roaming to allow supplicant to take care of
	 * roaming.
	 */
	brcmf_dbg(INFO, "Internal Roaming = %s\n",
		  ifp->drvr->settings->roamoff ? "Off" : "On");
	err = brcmf_fil_iovar_int_set(ifp, "roam_off",
				      ifp->drvr->settings->roamoff);
	if (err) {
		brcmf_err("roam_off error (%d)\n", err);
		goto roam_setup_done;
	}

	roamtrigger[0] = cpu_to_le32(WL_ROAM_TRIGGER_LEVEL);
	roamtrigger[1] = cpu_to_le32(BRCM_BAND_ALL);
	err = brcmf_fil_cmd_data_set(ifp, BRCMF_C_SET_ROAM_TRIGGER,
				     (void *)roamtrigger, sizeof(roamtrigger));
	if (err) {
		brcmf_err("WLC_SET_ROAM_TRIGGER error (%d)\n", err);
		goto roam_setup_done;
	}

	roam_delta[0] = cpu_to_le32(WL_ROAM_DELTA);
	roam_delta[1] = cpu_to_le32(BRCM_BAND_ALL);
	err = brcmf_fil_cmd_data_set(ifp, BRCMF_C_SET_ROAM_DELTA,
				     (void *)roam_delta, sizeof(roam_delta));
	if (err) {
		brcmf_err("WLC_SET_ROAM_DELTA error (%d)\n", err);
		goto roam_setup_done;
	}

roam_setup_done:
	return err;
}
