int TS_MSG_IMPRINT_print_bio(BIO *bio, TS_MSG_IMPRINT *a)
{
    ASN1_OCTET_STRING *msg;

    TS_X509_ALGOR_print_bio(bio, a->hash_algo);

    BIO_printf(bio, "Message data:\n");
    msg = a->hashed_msg;
    BIO_dump_indent(bio, (const char *)ASN1_STRING_data(msg),
                    ASN1_STRING_length(msg), 4);

    return 1;
}
