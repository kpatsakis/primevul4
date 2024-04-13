void derive_combined_bipredictive_merging_candidates(const base_context* ctx,
                                                     const slice_segment_header* shdr,
                                                     PBMotion* inout_mergeCandList,
                                                     int* inout_numMergeCand,
                                                     int maxCandidates)
{
  if (*inout_numMergeCand>1 && *inout_numMergeCand < maxCandidates) {
    int numOrigMergeCand = *inout_numMergeCand;

    int numInputMergeCand = *inout_numMergeCand;
    int combIdx = 0;
    uint8_t combStop = false;

    while (!combStop) {
      int l0CandIdx = table_8_19[0][combIdx];
      int l1CandIdx = table_8_19[1][combIdx];

      if (l0CandIdx >= numInputMergeCand ||
          l1CandIdx >= numInputMergeCand) {
        assert(false); // bitstream error -> TODO: conceal error
      }

      PBMotion& l0Cand = inout_mergeCandList[l0CandIdx];
      PBMotion& l1Cand = inout_mergeCandList[l1CandIdx];

      logtrace(LogMotion,"add bipredictive merging candidate (combIdx:%d)\n",combIdx);
      logtrace(LogMotion,"l0Cand:\n"); logmvcand(l0Cand);
      logtrace(LogMotion,"l1Cand:\n"); logmvcand(l1Cand);

      const de265_image* img0 = l0Cand.predFlag[0] ? ctx->get_image(shdr->RefPicList[0][l0Cand.refIdx[0]]) : NULL;
      const de265_image* img1 = l1Cand.predFlag[1] ? ctx->get_image(shdr->RefPicList[1][l1Cand.refIdx[1]]) : NULL;

      if (l0Cand.predFlag[0] && !img0) {
        return; // TODO error
      }

      if (l1Cand.predFlag[1] && !img1) {
        return; // TODO error
      }

      if (l0Cand.predFlag[0] && l1Cand.predFlag[1] &&
          (img0->PicOrderCntVal != img1->PicOrderCntVal     ||
           l0Cand.mv[0].x != l1Cand.mv[1].x ||
           l0Cand.mv[0].y != l1Cand.mv[1].y)) {
        PBMotion& p = inout_mergeCandList[ *inout_numMergeCand ];
        p.refIdx[0] = l0Cand.refIdx[0];
        p.refIdx[1] = l1Cand.refIdx[1];
        p.predFlag[0] = l0Cand.predFlag[0];
        p.predFlag[1] = l1Cand.predFlag[1];
        p.mv[0] = l0Cand.mv[0];
        p.mv[1] = l1Cand.mv[1];
        (*inout_numMergeCand)++;

        logtrace(LogMotion,"result:\n");
        logmvcand(p);
      }

      combIdx++;
      if (combIdx == numOrigMergeCand*(numOrigMergeCand-1) ||
          *inout_numMergeCand == maxCandidates) {
        combStop = true;
      }
    }
  }
}