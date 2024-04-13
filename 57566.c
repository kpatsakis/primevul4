int dns_packet_read_rr(DnsPacket *p, DnsResourceRecord **ret, bool *ret_cache_flush, size_t *start) {
        _cleanup_(dns_resource_record_unrefp) DnsResourceRecord *rr = NULL;
        _cleanup_(dns_resource_key_unrefp) DnsResourceKey *key = NULL;
        _cleanup_(rewind_dns_packet) DnsPacketRewinder rewinder;
        size_t offset;
        uint16_t rdlength;
        bool cache_flush;
        int r;

        assert(p);
        assert(ret);

        INIT_REWINDER(rewinder, p);

        r = dns_packet_read_key(p, &key, &cache_flush, NULL);
        if (r < 0)
                return r;

        if (!dns_class_is_valid_rr(key->class) || !dns_type_is_valid_rr(key->type))
                return -EBADMSG;

        rr = dns_resource_record_new(key);
        if (!rr)
                return -ENOMEM;

        r = dns_packet_read_uint32(p, &rr->ttl, NULL);
        if (r < 0)
                return r;

        /* RFC 2181, Section 8, suggests to
         * treat a TTL with the MSB set as a zero TTL. */
        if (rr->ttl & UINT32_C(0x80000000))
                rr->ttl = 0;

        r = dns_packet_read_uint16(p, &rdlength, NULL);
        if (r < 0)
                return r;

        if (p->rindex + rdlength > p->size)
                return -EBADMSG;

        offset = p->rindex;

        switch (rr->key->type) {

        case DNS_TYPE_SRV:
                r = dns_packet_read_uint16(p, &rr->srv.priority, NULL);
                if (r < 0)
                        return r;
                r = dns_packet_read_uint16(p, &rr->srv.weight, NULL);
                if (r < 0)
                        return r;
                r = dns_packet_read_uint16(p, &rr->srv.port, NULL);
                if (r < 0)
                        return r;
                r = dns_packet_read_name(p, &rr->srv.name, true, NULL);
                break;

        case DNS_TYPE_PTR:
        case DNS_TYPE_NS:
        case DNS_TYPE_CNAME:
        case DNS_TYPE_DNAME:
                r = dns_packet_read_name(p, &rr->ptr.name, true, NULL);
                break;

        case DNS_TYPE_HINFO:
                r = dns_packet_read_string(p, &rr->hinfo.cpu, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_string(p, &rr->hinfo.os, NULL);
                break;

        case DNS_TYPE_SPF: /* exactly the same as TXT */
        case DNS_TYPE_TXT:
                if (rdlength <= 0) {
                        DnsTxtItem *i;
                        /* RFC 6763, section 6.1 suggests to treat
                         * empty TXT RRs as equivalent to a TXT record
                         * with a single empty string. */

                        i = malloc0(offsetof(DnsTxtItem, data) + 1); /* for safety reasons we add an extra NUL byte */
                        if (!i)
                                return -ENOMEM;

                        rr->txt.items = i;
                } else {
                        DnsTxtItem *last = NULL;

                        while (p->rindex < offset + rdlength) {
                                DnsTxtItem *i;
                                const void *data;
                                size_t sz;

                                r = dns_packet_read_raw_string(p, &data, &sz, NULL);
                                if (r < 0)
                                        return r;

                                i = malloc0(offsetof(DnsTxtItem, data) + sz + 1); /* extra NUL byte at the end */
                                if (!i)
                                        return -ENOMEM;

                                memcpy(i->data, data, sz);
                                i->length = sz;

                                LIST_INSERT_AFTER(items, rr->txt.items, last, i);
                                last = i;
                        }
                }

                r = 0;
                break;

        case DNS_TYPE_A:
                r = dns_packet_read_blob(p, &rr->a.in_addr, sizeof(struct in_addr), NULL);
                break;

        case DNS_TYPE_AAAA:
                r = dns_packet_read_blob(p, &rr->aaaa.in6_addr, sizeof(struct in6_addr), NULL);
                break;

        case DNS_TYPE_SOA:
                r = dns_packet_read_name(p, &rr->soa.mname, true, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_name(p, &rr->soa.rname, true, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint32(p, &rr->soa.serial, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint32(p, &rr->soa.refresh, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint32(p, &rr->soa.retry, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint32(p, &rr->soa.expire, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint32(p, &rr->soa.minimum, NULL);
                break;

        case DNS_TYPE_MX:
                r = dns_packet_read_uint16(p, &rr->mx.priority, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_name(p, &rr->mx.exchange, true, NULL);
                break;

        case DNS_TYPE_LOC: {
                uint8_t t;
                size_t pos;

                r = dns_packet_read_uint8(p, &t, &pos);
                if (r < 0)
                        return r;

                if (t == 0) {
                        rr->loc.version = t;

                        r = dns_packet_read_uint8(p, &rr->loc.size, NULL);
                        if (r < 0)
                                return r;

                        if (!loc_size_ok(rr->loc.size))
                                return -EBADMSG;

                        r = dns_packet_read_uint8(p, &rr->loc.horiz_pre, NULL);
                        if (r < 0)
                                return r;

                        if (!loc_size_ok(rr->loc.horiz_pre))
                                return -EBADMSG;

                        r = dns_packet_read_uint8(p, &rr->loc.vert_pre, NULL);
                        if (r < 0)
                                return r;

                        if (!loc_size_ok(rr->loc.vert_pre))
                                return -EBADMSG;

                        r = dns_packet_read_uint32(p, &rr->loc.latitude, NULL);
                        if (r < 0)
                                return r;

                        r = dns_packet_read_uint32(p, &rr->loc.longitude, NULL);
                        if (r < 0)
                                return r;

                        r = dns_packet_read_uint32(p, &rr->loc.altitude, NULL);
                        if (r < 0)
                                return r;

                        break;
                } else {
                        dns_packet_rewind(p, pos);
                        rr->unparseable = true;
                        goto unparseable;
                }
        }

        case DNS_TYPE_DS:
                r = dns_packet_read_uint16(p, &rr->ds.key_tag, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint8(p, &rr->ds.algorithm, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint8(p, &rr->ds.digest_type, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_memdup(p, rdlength - 4,
                                           &rr->ds.digest, &rr->ds.digest_size,
                                           NULL);
                if (r < 0)
                        return r;

                if (rr->ds.digest_size <= 0)
                        /* the accepted size depends on the algorithm, but for now
                           just ensure that the value is greater than zero */
                        return -EBADMSG;

                break;

        case DNS_TYPE_SSHFP:
                r = dns_packet_read_uint8(p, &rr->sshfp.algorithm, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint8(p, &rr->sshfp.fptype, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_memdup(p, rdlength - 2,
                                           &rr->sshfp.fingerprint, &rr->sshfp.fingerprint_size,
                                           NULL);

                if (rr->sshfp.fingerprint_size <= 0)
                        /* the accepted size depends on the algorithm, but for now
                           just ensure that the value is greater than zero */
                        return -EBADMSG;

                break;

        case DNS_TYPE_DNSKEY:
                r = dns_packet_read_uint16(p, &rr->dnskey.flags, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint8(p, &rr->dnskey.protocol, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint8(p, &rr->dnskey.algorithm, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_memdup(p, rdlength - 4,
                                           &rr->dnskey.key, &rr->dnskey.key_size,
                                           NULL);

                if (rr->dnskey.key_size <= 0)
                        /* the accepted size depends on the algorithm, but for now
                           just ensure that the value is greater than zero */
                        return -EBADMSG;

                break;

        case DNS_TYPE_RRSIG:
                r = dns_packet_read_uint16(p, &rr->rrsig.type_covered, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint8(p, &rr->rrsig.algorithm, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint8(p, &rr->rrsig.labels, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint32(p, &rr->rrsig.original_ttl, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint32(p, &rr->rrsig.expiration, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint32(p, &rr->rrsig.inception, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint16(p, &rr->rrsig.key_tag, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_name(p, &rr->rrsig.signer, false, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_memdup(p, offset + rdlength - p->rindex,
                                           &rr->rrsig.signature, &rr->rrsig.signature_size,
                                           NULL);

                if (rr->rrsig.signature_size <= 0)
                        /* the accepted size depends on the algorithm, but for now
                           just ensure that the value is greater than zero */
                        return -EBADMSG;

                break;

        case DNS_TYPE_NSEC: {

                /*
                 * RFC6762, section 18.14 explictly states mDNS should use name compression.
                 * This contradicts RFC3845, section 2.1.1
                 */

                bool allow_compressed = p->protocol == DNS_PROTOCOL_MDNS;

                r = dns_packet_read_name(p, &rr->nsec.next_domain_name, allow_compressed, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_type_windows(p, &rr->nsec.types, offset + rdlength - p->rindex, NULL);

                /* We accept empty NSEC bitmaps. The bit indicating the presence of the NSEC record itself
                 * is redundant and in e.g., RFC4956 this fact is used to define a use for NSEC records
                 * without the NSEC bit set. */

                break;
        }
        case DNS_TYPE_NSEC3: {
                uint8_t size;

                r = dns_packet_read_uint8(p, &rr->nsec3.algorithm, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint8(p, &rr->nsec3.flags, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint16(p, &rr->nsec3.iterations, NULL);
                if (r < 0)
                        return r;

                /* this may be zero */
                r = dns_packet_read_uint8(p, &size, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_memdup(p, size, &rr->nsec3.salt, &rr->nsec3.salt_size, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint8(p, &size, NULL);
                if (r < 0)
                        return r;

                if (size <= 0)
                        return -EBADMSG;

                r = dns_packet_read_memdup(p, size,
                                           &rr->nsec3.next_hashed_name, &rr->nsec3.next_hashed_name_size,
                                           NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_type_windows(p, &rr->nsec3.types, offset + rdlength - p->rindex, NULL);

                /* empty non-terminals can have NSEC3 records, so empty bitmaps are allowed */

                break;
        }

        case DNS_TYPE_TLSA:
                r = dns_packet_read_uint8(p, &rr->tlsa.cert_usage, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint8(p, &rr->tlsa.selector, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_uint8(p, &rr->tlsa.matching_type, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_memdup(p, rdlength - 3,
                                           &rr->tlsa.data, &rr->tlsa.data_size,
                                           NULL);

                if (rr->tlsa.data_size <= 0)
                        /* the accepted size depends on the algorithm, but for now
                           just ensure that the value is greater than zero */
                        return -EBADMSG;

                break;

        case DNS_TYPE_CAA:
                r = dns_packet_read_uint8(p, &rr->caa.flags, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_string(p, &rr->caa.tag, NULL);
                if (r < 0)
                        return r;

                r = dns_packet_read_memdup(p,
                                           rdlength + offset - p->rindex,
                                           &rr->caa.value, &rr->caa.value_size, NULL);

                break;

        case DNS_TYPE_OPT: /* we only care about the header of OPT for now. */
        case DNS_TYPE_OPENPGPKEY:
        default:
        unparseable:
                r = dns_packet_read_memdup(p, rdlength, &rr->generic.data, &rr->generic.data_size, NULL);

                break;
        }
        if (r < 0)
                return r;
        if (p->rindex != offset + rdlength)
                return -EBADMSG;

        *ret = rr;
        rr = NULL;

        if (ret_cache_flush)
                *ret_cache_flush = cache_flush;
        if (start)
                *start = rewinder.saved_rindex;
        CANCEL_REWINDER(rewinder);

        return 0;
}
