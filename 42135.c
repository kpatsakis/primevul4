pkinit_octetstring2key(krb5_context context,
                       krb5_enctype etype,
                       unsigned char *key,
                       unsigned int dh_key_len,
                       krb5_keyblock *key_block)
{
    krb5_error_code retval;
    unsigned char *buf = NULL;
    unsigned char md[SHA_DIGEST_LENGTH];
    unsigned char counter;
    size_t keybytes, keylength, offset;
    krb5_data random_data;

    if ((buf = malloc(dh_key_len)) == NULL) {
        retval = ENOMEM;
        goto cleanup;
    }
    memset(buf, 0, dh_key_len);

    counter = 0;
    offset = 0;
    do {
        SHA_CTX c;

        SHA1_Init(&c);
        SHA1_Update(&c, &counter, 1);
        SHA1_Update(&c, key, dh_key_len);
        SHA1_Final(md, &c);

        if (dh_key_len - offset < sizeof(md))
            memcpy(buf + offset, md, dh_key_len - offset);
        else
            memcpy(buf + offset, md, sizeof(md));

        offset += sizeof(md);
        counter++;
    } while (offset < dh_key_len);

    key_block->magic = 0;
    key_block->enctype = etype;

    retval = krb5_c_keylengths(context, etype, &keybytes, &keylength);
    if (retval)
        goto cleanup;

    key_block->length = keylength;
    key_block->contents = malloc(keylength);
    if (key_block->contents == NULL) {
        retval = ENOMEM;
        goto cleanup;
    }

    random_data.length = keybytes;
    random_data.data = (char *)buf;

    retval = krb5_c_random_to_key(context, etype, &random_data, key_block);

cleanup:
    free(buf);
    /* If this is an error return, free the allocated keyblock, if any */
    if (retval) {
        krb5_free_keyblock_contents(context, key_block);
    }

    return retval;
}
