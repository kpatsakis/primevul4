static bool opt_is_good(DnsResourceRecord *rr, bool *rfc6975) {
        const uint8_t* p;
        bool found_dau_dhu_n3u = false;
        size_t l;

        /* Checks whether the specified OPT RR is well-formed and whether it contains RFC6975 data (which is not OK in
         * a reply). */

        assert(rr);
        assert(rr->key->type == DNS_TYPE_OPT);

        /* Check that the version is 0 */
        if (((rr->ttl >> 16) & UINT32_C(0xFF)) != 0) {
                *rfc6975 = false;
                return true; /* if it's not version 0, it's OK, but we will ignore the OPT field contents */
        }

        p = rr->opt.data;
        l = rr->opt.data_size;
        while (l > 0) {
                uint16_t option_code, option_length;

                /* At least four bytes for OPTION-CODE and OPTION-LENGTH are required */
                if (l < 4U)
                        return false;

                option_code = unaligned_read_be16(p);
                option_length = unaligned_read_be16(p + 2);

                if (l < option_length + 4U)
                        return false;

                /* RFC 6975 DAU, DHU or N3U fields found. */
                if (IN_SET(option_code, 5, 6, 7))
                        found_dau_dhu_n3u = true;

                p += option_length + 4U;
                l -= option_length + 4U;
        }

        *rfc6975 = found_dau_dhu_n3u;
        return true;
}
