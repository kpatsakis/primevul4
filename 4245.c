void fill_luma_motion_vector_predictors(base_context* ctx,
                                        const slice_segment_header* shdr,
                                        de265_image* img,
                                        int xC,int yC,int nCS,int xP,int yP,
                                        int nPbW,int nPbH, int l,
                                        int refIdx, int partIdx,
                                        MotionVector out_mvpList[2])
{
  // 8.5.3.1.6: derive two spatial vector predictors A (0) and B (1)

  uint8_t availableFlagLXN[2];
  MotionVector mvLXN[2];

  derive_spatial_luma_vector_prediction(ctx, img, shdr, xC,yC, nCS, xP,yP,
                                        nPbW,nPbH, l, refIdx, partIdx,
                                        availableFlagLXN, mvLXN);

  // 8.5.3.1.7: if we only have one spatial vector or both spatial vectors are the same,
  // derive a temporal predictor

  uint8_t availableFlagLXCol;
  MotionVector mvLXCol;


  if (availableFlagLXN[0] &&
      availableFlagLXN[1] &&
      (mvLXN[0].x != mvLXN[1].x || mvLXN[0].y != mvLXN[1].y)) {
    availableFlagLXCol = 0;
  }
  else {
    derive_temporal_luma_vector_prediction(ctx, img, shdr,
                                           xP,yP, nPbW,nPbH, refIdx,l,
                                           &mvLXCol, &availableFlagLXCol);
  }


  // --- build candidate vector list with exactly two entries ---

  int numMVPCandLX=0;

  // spatial predictor A

  if (availableFlagLXN[0])
    {
      out_mvpList[numMVPCandLX++] = mvLXN[0];
    }

  // spatial predictor B (if not same as A)

  if (availableFlagLXN[1] &&
      (!availableFlagLXN[0] || // in case A in not available, but mvLXA initialized to same as mvLXB
       (mvLXN[0].x != mvLXN[1].x || mvLXN[0].y != mvLXN[1].y)))
    {
      out_mvpList[numMVPCandLX++] = mvLXN[1];
    }

  // temporal predictor

  if (availableFlagLXCol)
    {
      out_mvpList[numMVPCandLX++] = mvLXCol;
    }

  // fill with zero predictors

  while (numMVPCandLX<2) {
    out_mvpList[numMVPCandLX].x = 0;
    out_mvpList[numMVPCandLX].y = 0;
    numMVPCandLX++;
  }


  assert(numMVPCandLX==2);
}