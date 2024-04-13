int dns_packet_extract(DnsPacket *p) {
        _cleanup_(dns_question_unrefp) DnsQuestion *question = NULL;
        _cleanup_(dns_answer_unrefp) DnsAnswer *answer = NULL;
        _cleanup_(rewind_dns_packet) DnsPacketRewinder rewinder = {};
        unsigned n, i;
        int r;

        if (p->extracted)
                return 0;

        INIT_REWINDER(rewinder, p);
        dns_packet_rewind(p, DNS_PACKET_HEADER_SIZE);

        n = DNS_PACKET_QDCOUNT(p);
        if (n > 0) {
                question = dns_question_new(n);
                if (!question)
                        return -ENOMEM;

                for (i = 0; i < n; i++) {
                        _cleanup_(dns_resource_key_unrefp) DnsResourceKey *key = NULL;
                        bool cache_flush;

                        r = dns_packet_read_key(p, &key, &cache_flush, NULL);
                        if (r < 0)
                                return r;

                        if (cache_flush)
                                return -EBADMSG;

                        if (!dns_type_is_valid_query(key->type))
                                return -EBADMSG;

                        r = dns_question_add(question, key);
                        if (r < 0)
                                return r;
                }
        }

        n = DNS_PACKET_RRCOUNT(p);
        if (n > 0) {
                _cleanup_(dns_resource_record_unrefp) DnsResourceRecord *previous = NULL;
                bool bad_opt = false;

                answer = dns_answer_new(n);
                if (!answer)
                        return -ENOMEM;

                for (i = 0; i < n; i++) {
                        _cleanup_(dns_resource_record_unrefp) DnsResourceRecord *rr = NULL;
                        bool cache_flush = false;

                        r = dns_packet_read_rr(p, &rr, &cache_flush, NULL);
                        if (r < 0)
                                return r;

                        /* Try to reduce memory usage a bit */
                        if (previous)
                                dns_resource_key_reduce(&rr->key, &previous->key);

                        if (rr->key->type == DNS_TYPE_OPT) {
                                bool has_rfc6975;

                                if (p->opt || bad_opt) {
                                        /* Multiple OPT RRs? if so, let's ignore all, because there's something wrong
                                         * with the server, and if one is valid we wouldn't know which one. */
                                        log_debug("Multiple OPT RRs detected, ignoring all.");
                                        bad_opt = true;
                                        continue;
                                }

                                if (!dns_name_is_root(dns_resource_key_name(rr->key))) {
                                        /* If the OPT RR is not owned by the root domain, then it is bad, let's ignore
                                         * it. */
                                        log_debug("OPT RR is not owned by root domain, ignoring.");
                                        bad_opt = true;
                                        continue;
                                }

                                if (i < DNS_PACKET_ANCOUNT(p) + DNS_PACKET_NSCOUNT(p)) {
                                        /* OPT RR is in the wrong section? Some Belkin routers do this. This is a hint
                                         * the EDNS implementation is borked, like the Belkin one is, hence ignore
                                         * it. */
                                        log_debug("OPT RR in wrong section, ignoring.");
                                        bad_opt = true;
                                        continue;
                                }

                                if (!opt_is_good(rr, &has_rfc6975)) {
                                        log_debug("Malformed OPT RR, ignoring.");
                                        bad_opt = true;
                                        continue;
                                }

                                if (DNS_PACKET_QR(p)) {
                                        /* Additional checks for responses */

                                        if (!DNS_RESOURCE_RECORD_OPT_VERSION_SUPPORTED(rr)) {
                                                /* If this is a reply and we don't know the EDNS version then something
                                                 * is weird... */
                                                log_debug("EDNS version newer that our request, bad server.");
                                                return -EBADMSG;
                                        }

                                        if (has_rfc6975) {
                                                /* If the OPT RR contains RFC6975 algorithm data, then this is indication that
                                                 * the server just copied the OPT it got from us (which contained that data)
                                                 * back into the reply. If so, then it doesn't properly support EDNS, as
                                                 * RFC6975 makes it very clear that the algorithm data should only be contained
                                                 * in questions, never in replies. Crappy Belkin routers copy the OPT data for
                                                 * example, hence let's detect this so that we downgrade early. */
                                                log_debug("OPT RR contained RFC6975 data, ignoring.");
                                                bad_opt = true;
                                                continue;
                                        }
                                }

                                p->opt = dns_resource_record_ref(rr);
                        } else {

                                /* According to RFC 4795, section 2.9. only the RRs from the Answer section shall be
                                 * cached. Hence mark only those RRs as cacheable by default, but not the ones from the
                                 * Additional or Authority sections. */

                                r = dns_answer_add(answer, rr, p->ifindex,
                                                   (i < DNS_PACKET_ANCOUNT(p) ? DNS_ANSWER_CACHEABLE : 0) |
                                                   (p->protocol == DNS_PROTOCOL_MDNS && !cache_flush ? DNS_ANSWER_SHARED_OWNER : 0));
                                if (r < 0)
                                        return r;
                        }

                        /* Remember this RR, so that we potentically can merge it's ->key object with the next RR. Note
                         * that we only do this if we actually decided to keep the RR around. */
                        dns_resource_record_unref(previous);
                        previous = dns_resource_record_ref(rr);
                }

                if (bad_opt)
                        p->opt = dns_resource_record_unref(p->opt);
        }

        p->question = question;
        question = NULL;

        p->answer = answer;
        answer = NULL;

        p->extracted = true;

        /* no CANCEL, always rewind */
        return 0;
}
