static int64_t mov_read_atom_into_extradata(MOVContext *c, AVIOContext *pb, MOVAtom atom,
                                        AVCodecParameters *par, uint8_t *buf)
{
    int64_t result = atom.size;
    int err;

    AV_WB32(buf    , atom.size + 8);
    AV_WL32(buf + 4, atom.type);
    err = ffio_read_size(pb, buf + 8, atom.size);
    if (err < 0) {
        par->extradata_size -= atom.size;
        return err;
    } else if (err < atom.size) {
        av_log(c->fc, AV_LOG_WARNING, "truncated extradata\n");
        par->extradata_size -= atom.size - err;
        result = err;
    }
    memset(buf + 8 + err, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    return result;
}
