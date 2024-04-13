static void dns_packet_free(DnsPacket *p) {
        char *s;

        assert(p);

        dns_question_unref(p->question);
        dns_answer_unref(p->answer);
        dns_resource_record_unref(p->opt);

        while ((s = hashmap_steal_first_key(p->names)))
                free(s);
        hashmap_free(p->names);

        free(p->_data);

        if (!p->on_stack)
                free(p);
}
