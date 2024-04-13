enum AVChromaLocation avcodec_chroma_pos_to_enum(int xpos, int ypos)
{
    int pos, xout, yout;

    for (pos = AVCHROMA_LOC_UNSPECIFIED + 1; pos < AVCHROMA_LOC_NB; pos++) {
        if (avcodec_enum_to_chroma_pos(&xout, &yout, pos) == 0 && xout == xpos && yout == ypos)
            return pos;
    }
    return AVCHROMA_LOC_UNSPECIFIED;
}
