static int asf_get_packet(AVFormatContext *s, AVIOContext *pb)
{
    ASFContext *asf = s->priv_data;
    uint32_t packet_length, padsize;
    int rsize = 8;
    int c, d, e, off;

    if (asf->uses_std_ecc > 0) {
        off = 32768;
        if (asf->no_resync_search)
            off = 3;

        c = d = e = -1;
        while (off-- > 0) {
            c = d;
            d = e;
            e = avio_r8(pb);
            if (c == 0x82 && !d && !e)
                break;
        }

        if (c != 0x82) {
            /* This code allows handling of -EAGAIN at packet boundaries (i.e.
            * if the packet sync code above triggers -EAGAIN). This does not
            * imply complete -EAGAIN handling support at random positions in
            * the stream. */
            if (pb->error == AVERROR(EAGAIN))
                return AVERROR(EAGAIN);
            if (!avio_feof(pb))
                av_log(s, AV_LOG_ERROR,
                    "ff asf bad header %x  at:%"PRId64"\n", c, avio_tell(pb));
        }
        if ((c & 0x8f) == 0x82) {
            if (d || e) {
                if (!avio_feof(pb))
                    av_log(s, AV_LOG_ERROR, "ff asf bad non zero\n");
                return AVERROR_INVALIDDATA;
            }
            c      = avio_r8(pb);
            d      = avio_r8(pb);
            rsize += 3;
        } else if(!avio_feof(pb)) {
            avio_seek(pb, -1, SEEK_CUR); // FIXME
        }
    } else {
        c = avio_r8(pb);
        if (c & 0x80) {
            rsize ++;
            if (!(c & 0x60)) {
                d = avio_r8(pb);
                e = avio_r8(pb);
                avio_seek(pb, (c & 0xF) - 2, SEEK_CUR);
                rsize += c & 0xF;
            }

            if (c != 0x82)
                avpriv_request_sample(s, "Invalid ECC byte");

            if (!asf->uses_std_ecc)
                asf->uses_std_ecc =  (c == 0x82 && !d && !e) ? 1 : -1;

            c = avio_r8(pb);
        } else
            asf->uses_std_ecc =  -1;
        d = avio_r8(pb);
    }

    asf->packet_flags    = c;
    asf->packet_property = d;

    DO_2BITS(asf->packet_flags >> 5, packet_length, s->packet_size);
    DO_2BITS(asf->packet_flags >> 1, padsize, 0); // sequence ignored
    DO_2BITS(asf->packet_flags >> 3, padsize, 0); // padding length

    if (!packet_length || packet_length >= (1U << 29)) {
        av_log(s, AV_LOG_ERROR,
               "invalid packet_length %"PRIu32" at:%"PRId64"\n",
               packet_length, avio_tell(pb));
        return AVERROR_INVALIDDATA;
    }
    if (padsize >= packet_length) {
        av_log(s, AV_LOG_ERROR,
               "invalid padsize %"PRIu32" at:%"PRId64"\n", padsize, avio_tell(pb));
        return AVERROR_INVALIDDATA;
    }

    asf->packet_timestamp = avio_rl32(pb);
    avio_rl16(pb); /* duration */

    if (asf->packet_flags & 0x01) {
        asf->packet_segsizetype = avio_r8(pb);
        rsize++;
        asf->packet_segments = asf->packet_segsizetype & 0x3f;
    } else {
        asf->packet_segments    = 1;
        asf->packet_segsizetype = 0x80;
    }
    if (rsize > packet_length - padsize) {
        asf->packet_size_left = 0;
        av_log(s, AV_LOG_ERROR,
               "invalid packet header length %d for pktlen %"PRIu32"-%"PRIu32" at %"PRId64"\n",
               rsize, packet_length, padsize, avio_tell(pb));
        return AVERROR_INVALIDDATA;
    }
    asf->packet_size_left = packet_length - padsize - rsize;
    if (packet_length < asf->hdr.min_pktsize)
        padsize += asf->hdr.min_pktsize - packet_length;
    asf->packet_padsize = padsize;
    av_log(s, AV_LOG_TRACE, "packet: size=%d padsize=%d  left=%d\n",
            s->packet_size, asf->packet_padsize, asf->packet_size_left);
    return 0;
}
