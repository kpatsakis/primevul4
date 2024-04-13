rsvp_clear_checksum(void *header)
{
    struct rsvp_common_header *rsvp_com_header = (struct rsvp_common_header *) header;

    rsvp_com_header->checksum[0] = 0;
    rsvp_com_header->checksum[1] = 0;
}
