pkinit_init_kdc_profile(krb5_context context, pkinit_kdc_context plgctx)
{
    krb5_error_code retval;
    char *eku_string = NULL;

    pkiDebug("%s: entered for realm %s\n", __FUNCTION__, plgctx->realmname);
    retval = pkinit_kdcdefault_string(context, plgctx->realmname,
                                      KRB5_CONF_PKINIT_IDENTITY,
                                      &plgctx->idopts->identity);
    if (retval != 0 || NULL == plgctx->idopts->identity) {
        retval = EINVAL;
        krb5_set_error_message(context, retval,
                               _("No pkinit_identity supplied for realm %s"),
                               plgctx->realmname);
        goto errout;
    }

    retval = pkinit_kdcdefault_strings(context, plgctx->realmname,
                                       KRB5_CONF_PKINIT_ANCHORS,
                                       &plgctx->idopts->anchors);
    if (retval != 0 || NULL == plgctx->idopts->anchors) {
        retval = EINVAL;
        krb5_set_error_message(context, retval,
                               _("No pkinit_anchors supplied for realm %s"),
                               plgctx->realmname);
        goto errout;
    }

    pkinit_kdcdefault_strings(context, plgctx->realmname,
                              KRB5_CONF_PKINIT_POOL,
                              &plgctx->idopts->intermediates);

    pkinit_kdcdefault_strings(context, plgctx->realmname,
                              KRB5_CONF_PKINIT_REVOKE,
                              &plgctx->idopts->crls);

    pkinit_kdcdefault_string(context, plgctx->realmname,
                             KRB5_CONF_PKINIT_KDC_OCSP,
                             &plgctx->idopts->ocsp);

    pkinit_kdcdefault_integer(context, plgctx->realmname,
                              KRB5_CONF_PKINIT_DH_MIN_BITS,
                              PKINIT_DEFAULT_DH_MIN_BITS,
                              &plgctx->opts->dh_min_bits);
    if (plgctx->opts->dh_min_bits < PKINIT_DH_MIN_CONFIG_BITS) {
        pkiDebug("%s: invalid value (%d < %d) for pkinit_dh_min_bits, "
                 "using default value (%d) instead\n", __FUNCTION__,
                 plgctx->opts->dh_min_bits, PKINIT_DH_MIN_CONFIG_BITS,
                 PKINIT_DEFAULT_DH_MIN_BITS);
        plgctx->opts->dh_min_bits = PKINIT_DEFAULT_DH_MIN_BITS;
    }

    pkinit_kdcdefault_boolean(context, plgctx->realmname,
                              KRB5_CONF_PKINIT_ALLOW_UPN,
                              0, &plgctx->opts->allow_upn);

    pkinit_kdcdefault_boolean(context, plgctx->realmname,
                              KRB5_CONF_PKINIT_REQUIRE_CRL_CHECKING,
                              0, &plgctx->opts->require_crl_checking);

    pkinit_kdcdefault_string(context, plgctx->realmname,
                             KRB5_CONF_PKINIT_EKU_CHECKING,
                             &eku_string);
    if (eku_string != NULL) {
        if (strcasecmp(eku_string, "kpClientAuth") == 0) {
            plgctx->opts->require_eku = 1;
            plgctx->opts->accept_secondary_eku = 0;
        } else if (strcasecmp(eku_string, "scLogin") == 0) {
            plgctx->opts->require_eku = 1;
            plgctx->opts->accept_secondary_eku = 1;
        } else if (strcasecmp(eku_string, "none") == 0) {
            plgctx->opts->require_eku = 0;
            plgctx->opts->accept_secondary_eku = 0;
        } else {
            pkiDebug("%s: Invalid value for pkinit_eku_checking: '%s'\n",
                     __FUNCTION__, eku_string);
        }
        free(eku_string);
    }


    return 0;
errout:
    pkinit_fini_kdc_profile(context, plgctx);
    return retval;
}
