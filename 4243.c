void mc_chroma(const base_context* ctx,
               const seq_parameter_set* sps,
               int mv_x, int mv_y,
               int xP,int yP,
               int16_t* out, int out_stride,
               const pixel_t* ref, int ref_stride,
               int nPbWC, int nPbHC, int bit_depth_C)
{
  // chroma sample interpolation process (8.5.3.2.2.2)

  //const int shift1 = sps->BitDepth_C-8;
  //const int shift2 = 6;
  const int shift3 = 14 - sps->BitDepth_C;

  int wC = sps->pic_width_in_luma_samples /sps->SubWidthC;
  int hC = sps->pic_height_in_luma_samples/sps->SubHeightC;

  mv_x *= 2 / sps->SubWidthC;
  mv_y *= 2 / sps->SubHeightC;

  int xFracC = mv_x & 7;
  int yFracC = mv_y & 7;

  int xIntOffsC = xP/sps->SubWidthC  + (mv_x>>3);
  int yIntOffsC = yP/sps->SubHeightC + (mv_y>>3);

  ALIGNED_32(int16_t mcbuffer[MAX_CU_SIZE*(MAX_CU_SIZE+7)]);

  if (xFracC == 0 && yFracC == 0) {
    if (xIntOffsC>=0 && nPbWC+xIntOffsC<=wC &&
        yIntOffsC>=0 && nPbHC+yIntOffsC<=hC) {
      ctx->acceleration.put_hevc_epel(out, out_stride,
                                      &ref[xIntOffsC + yIntOffsC*ref_stride], ref_stride,
                                      nPbWC,nPbHC, 0,0, NULL, bit_depth_C);
    }
    else
      {
        for (int y=0;y<nPbHC;y++)
          for (int x=0;x<nPbWC;x++) {

            int xB = Clip3(0,wC-1,x + xIntOffsC);
            int yB = Clip3(0,hC-1,y + yIntOffsC);

            out[y*out_stride+x] = ref[ xB + yB*ref_stride ] << shift3;
          }
      }
  }
  else {
    pixel_t padbuf[(MAX_CU_SIZE+16)*(MAX_CU_SIZE+3)];

    const pixel_t* src_ptr;
    int src_stride;

    int extra_top  = 1;
    int extra_left = 1;
    int extra_right  = 2;
    int extra_bottom = 2;

    if (xIntOffsC>=1 && nPbWC+xIntOffsC<=wC-2 &&
        yIntOffsC>=1 && nPbHC+yIntOffsC<=hC-2) {
      src_ptr = &ref[xIntOffsC + yIntOffsC*ref_stride];
      src_stride = ref_stride;
    }
    else {
      for (int y=-extra_top;y<nPbHC+extra_bottom;y++) {
        for (int x=-extra_left;x<nPbWC+extra_right;x++) {

          int xA = Clip3(0,wC-1,x + xIntOffsC);
          int yA = Clip3(0,hC-1,y + yIntOffsC);

          padbuf[x+extra_left + (y+extra_top)*(MAX_CU_SIZE+16)] = ref[ xA + yA*ref_stride ];
        }
      }

      src_ptr = &padbuf[extra_left + extra_top*(MAX_CU_SIZE+16)];
      src_stride = MAX_CU_SIZE+16;
    }


    if (xFracC && yFracC) {
      ctx->acceleration.put_hevc_epel_hv(out, out_stride,
                                         src_ptr, src_stride,
                                         nPbWC,nPbHC, xFracC,yFracC, mcbuffer, bit_depth_C);
    }
    else if (xFracC) {
      ctx->acceleration.put_hevc_epel_h(out, out_stride,
                                        src_ptr, src_stride,
                                        nPbWC,nPbHC, xFracC,yFracC, mcbuffer, bit_depth_C);
    }
    else if (yFracC) {
      ctx->acceleration.put_hevc_epel_v(out, out_stride,
                                        src_ptr, src_stride,
                                        nPbWC,nPbHC, xFracC,yFracC, mcbuffer, bit_depth_C);
    }
    else {
      assert(false); // full-pel shifts are handled above
    }
  }
}