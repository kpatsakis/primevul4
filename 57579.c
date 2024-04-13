static void rewind_dns_packet(DnsPacketRewinder *rewinder) {
        if (rewinder->packet)
                dns_packet_rewind(rewinder->packet, rewinder->saved_rindex);
}
