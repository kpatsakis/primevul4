verify_client_eku(krb5_context context,
                  pkinit_kdc_context plgctx,
                  pkinit_kdc_req_context reqctx,
                  int *eku_accepted)
{
    krb5_error_code retval;

    *eku_accepted = 0;

    if (plgctx->opts->require_eku == 0) {
        pkiDebug("%s: configuration requests no EKU checking\n", __FUNCTION__);
        *eku_accepted = 1;
        retval = 0;
        goto out;
    }

    retval = crypto_check_cert_eku(context, plgctx->cryptoctx,
                                   reqctx->cryptoctx, plgctx->idctx,
                                   0, /* kdc cert */
                                   plgctx->opts->accept_secondary_eku,
                                   eku_accepted);
    if (retval) {
        pkiDebug("%s: Error from crypto_check_cert_eku %d (%s)\n",
                 __FUNCTION__, retval, error_message(retval));
        goto out;
    }

out:
    pkiDebug("%s: returning retval %d, eku_accepted %d\n",
             __FUNCTION__, retval, *eku_accepted);
    return retval;
}
