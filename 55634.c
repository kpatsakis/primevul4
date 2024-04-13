static int mxf_probe(AVProbeData *p) {
    const uint8_t *bufp = p->buf;
    const uint8_t *end = p->buf + p->buf_size;

    if (p->buf_size < sizeof(mxf_header_partition_pack_key))
        return 0;

    /* Must skip Run-In Sequence and search for MXF header partition pack key SMPTE 377M 5.5 */
    end -= sizeof(mxf_header_partition_pack_key);

    for (; bufp < end;) {
        if (!((bufp[13] - 1) & 0xF2)){
            if (AV_RN32(bufp   ) == AV_RN32(mxf_header_partition_pack_key   ) &&
                AV_RN32(bufp+ 4) == AV_RN32(mxf_header_partition_pack_key+ 4) &&
                AV_RN32(bufp+ 8) == AV_RN32(mxf_header_partition_pack_key+ 8) &&
                AV_RN16(bufp+12) == AV_RN16(mxf_header_partition_pack_key+12))
                return AVPROBE_SCORE_MAX;
            bufp ++;
        } else
            bufp += 10;
    }

    return 0;
}
