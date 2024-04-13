void LibRaw::foveon_thumb_loader (void)
{
    unsigned bwide, row, col, bitbuf=0, bit=1, c, i;
    struct decode *dindex;
    short pred[3];
    
    if(T.thumb) free(T.thumb);
    T.thumb = NULL;
    
    bwide = get4();
    if (bwide > 0) 
        {
            if (bwide < (unsigned)T.twidth*3) return;
            T.thumb = (char*)malloc(3*T.twidth * T.theight);
            merror (T.thumb, "foveon_thumb()");
            char *buf = (char*)malloc(bwide); 
            merror (buf, "foveon_thumb()");
            for (row=0; row < T.theight; row++) 
                {
                    ID.input->read(buf, 1, bwide);
                    memmove(T.thumb+(row*T.twidth*3),buf,T.twidth*3);
                }
            free(buf);
            T.tlength = 3*T.twidth * T.theight;
            T.tformat = LIBRAW_THUMBNAIL_BITMAP;
            return;
        }
    else 
        {
            foveon_decoder (256, 0);
            T.thumb = (char*)malloc(3*T.twidth * T.theight);
            char *bufp = T.thumb;
            merror (T.thumb, "foveon_thumb()");
            for (row=0; row < T.theight; row++) 
                {
                    memset (pred, 0, sizeof pred);
                    if (!bit) get4();
                    for (bit=col=0; col < T.twidth; col++)
                        for(c=0;c<3;c++) 
                            {
                                for (dindex=first_decode; dindex->branch[0]; ) 
                                    {
                                        if ((bit = (bit-1) & 31) == 31)
                                            for (i=0; i < 4; i++)
                                                bitbuf = (bitbuf << 8) + ID.input->get_char();
                                        dindex = dindex->branch[bitbuf >> bit & 1];
                                    }
                                pred[c] += dindex->leaf;
                                (*bufp++)=pred[c];
                            }
                }
            T.tformat = LIBRAW_THUMBNAIL_BITMAP;
            T.tlength = 3*T.twidth * T.theight;
        }
    return;
}
