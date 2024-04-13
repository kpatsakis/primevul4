pkcs7_decrypt(krb5_context context,
              pkinit_identity_crypto_context id_cryptoctx,
              PKCS7 *p7,
              BIO *data)
{
    BIO *tmpmem = NULL;
    int retval = 0, i = 0;
    char buf[4096];

    if(p7 == NULL)
        return 0;

    if(!PKCS7_type_is_enveloped(p7)) {
        pkiDebug("wrong pkcs7 content type\n");
        return 0;
    }

    if(!(tmpmem = pkcs7_dataDecode(context, id_cryptoctx, p7))) {
        pkiDebug("unable to decrypt pkcs7 object\n");
        return 0;
    }

    for(;;) {
        i = BIO_read(tmpmem, buf, sizeof(buf));
        if (i <= 0) break;
        BIO_write(data, buf, i);
        BIO_free_all(tmpmem);
        return 1;
    }
    return retval;
}
