void mc_luma(const base_context* ctx,
             const seq_parameter_set* sps, int mv_x, int mv_y,
             int xP,int yP,
             int16_t* out, int out_stride,
             const pixel_t* ref, int ref_stride,
             int nPbW, int nPbH, int bitDepth_L)
{
  int xFracL = mv_x & 3;
  int yFracL = mv_y & 3;

  int xIntOffsL = xP + (mv_x>>2);
  int yIntOffsL = yP + (mv_y>>2);

  // luma sample interpolation process (8.5.3.2.2.1)

  //const int shift1 = sps->BitDepth_Y-8;
  //const int shift2 = 6;
  const int shift3 = 14 - sps->BitDepth_Y;

  int w = sps->pic_width_in_luma_samples;
  int h = sps->pic_height_in_luma_samples;

  ALIGNED_16(int16_t) mcbuffer[MAX_CU_SIZE * (MAX_CU_SIZE+7)];

  if (xFracL==0 && yFracL==0) {

    if (xIntOffsL >= 0 && yIntOffsL >= 0 &&
        nPbW+xIntOffsL <= w && nPbH+yIntOffsL <= h) {

      ctx->acceleration.put_hevc_qpel(out, out_stride,
                                      &ref[yIntOffsL*ref_stride + xIntOffsL],
                                      ref_stride /* sizeof(pixel_t)*/,
                                      nPbW,nPbH, mcbuffer, 0,0, bitDepth_L);
    }
    else {
      for (int y=0;y<nPbH;y++)
        for (int x=0;x<nPbW;x++) {

          int xA = Clip3(0,w-1,x + xIntOffsL);
          int yA = Clip3(0,h-1,y + yIntOffsL);

          out[y*out_stride+x] = ref[ xA + yA*ref_stride ] << shift3;
        }
    }

#ifdef DE265_LOG_TRACE
    logtrace(LogMotion,"---MC luma %d %d = direct---\n",xFracL,yFracL);

    for (int y=0;y<nPbH;y++) {
      for (int x=0;x<nPbW;x++) {

        int xA = Clip3(0,w-1,x + xIntOffsL);
        int yA = Clip3(0,h-1,y + yIntOffsL);

        logtrace(LogMotion,"%02x ", ref[ xA + yA*ref_stride ]);
      }
      logtrace(LogMotion,"\n");
    }

    logtrace(LogMotion," -> \n");

    for (int y=0;y<nPbH;y++) {
      for (int x=0;x<nPbW;x++) {

        logtrace(LogMotion,"%02x ",out[y*out_stride+x] >> 6); // 6 will be used when summing predictions
      }
      logtrace(LogMotion,"\n");
    }
#endif
  }
  else {
    int extra_left   = extra_before[xFracL];
    int extra_right  = extra_after [xFracL];
    int extra_top    = extra_before[yFracL];
    int extra_bottom = extra_after [yFracL];

    //int nPbW_extra = extra_left + nPbW + extra_right;
    //int nPbH_extra = extra_top  + nPbH + extra_bottom;


    pixel_t padbuf[(MAX_CU_SIZE+16)*(MAX_CU_SIZE+7)];

    const pixel_t* src_ptr;
    int src_stride;

    if (-extra_left + xIntOffsL >= 0 &&
        -extra_top  + yIntOffsL >= 0 &&
        nPbW+extra_right  + xIntOffsL < w &&
        nPbH+extra_bottom + yIntOffsL < h) {
      src_ptr = &ref[xIntOffsL + yIntOffsL*ref_stride];
      src_stride = ref_stride;
    }
    else {
      for (int y=-extra_top;y<nPbH+extra_bottom;y++) {
        for (int x=-extra_left;x<nPbW+extra_right;x++) {

          int xA = Clip3(0,w-1,x + xIntOffsL);
          int yA = Clip3(0,h-1,y + yIntOffsL);

          padbuf[x+extra_left + (y+extra_top)*(MAX_CU_SIZE+16)] = ref[ xA + yA*ref_stride ];
        }
      }

      src_ptr = &padbuf[extra_top*(MAX_CU_SIZE+16) + extra_left];
      src_stride = MAX_CU_SIZE+16;
    }

    ctx->acceleration.put_hevc_qpel(out, out_stride,
                                    src_ptr, src_stride /* sizeof(pixel_t) */,
                                    nPbW,nPbH, mcbuffer, xFracL,yFracL, bitDepth_L);


    logtrace(LogMotion,"---V---\n");
    for (int y=0;y<nPbH;y++) {
      for (int x=0;x<nPbW;x++) {
        logtrace(LogMotion,"%04x ",out[x+y*out_stride]);
      }
      logtrace(LogMotion,"\n");
    }
  }
}