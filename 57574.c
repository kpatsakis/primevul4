DnsPacket *dns_packet_unref(DnsPacket *p) {
        if (!p)
                return NULL;

        assert(p->n_ref > 0);

        dns_packet_unref(p->more);

        if (p->n_ref == 1)
                dns_packet_free(p);
        else
                p->n_ref--;

        return NULL;
}
