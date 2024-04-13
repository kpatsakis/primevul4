openssl_init()
{
    static int did_init = 0;

    if (!did_init) {
        /* initialize openssl routines */
        CRYPTO_malloc_init();
        ERR_load_crypto_strings();
        OpenSSL_add_all_algorithms();
        did_init++;
    }
}
