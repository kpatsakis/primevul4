int TS_X509_ALGOR_print_bio(BIO *bio, const X509_ALGOR *alg)
{
    int i = OBJ_obj2nid(alg->algorithm);
    return BIO_printf(bio, "Hash Algorithm: %s\n",
                      (i == NID_undef) ? "UNKNOWN" : OBJ_nid2ln(i));
}
