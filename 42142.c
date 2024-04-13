print_dh(DH * dh, char *msg)
{
    BIO *bio_err = NULL;

    bio_err = BIO_new(BIO_s_file());
    BIO_set_fp(bio_err, stderr, BIO_NOCLOSE | BIO_FP_TEXT);

    if (msg)
        BIO_puts(bio_err, (const char *)msg);
    if (dh)
        DHparams_print(bio_err, dh);

    BIO_puts(bio_err, "private key: ");
    BN_print(bio_err, dh->priv_key);
    BIO_puts(bio_err, (const char *)"\n");
    BIO_free(bio_err);

}
