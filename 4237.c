void derive_zero_motion_vector_candidates(const slice_segment_header* shdr,
                                          PBMotion* out_mergeCandList,
                                          int* inout_numCurrMergeCand,
                                          int maxCandidates)
{
  logtrace(LogMotion,"derive_zero_motion_vector_candidates\n");

  int numRefIdx;

  if (shdr->slice_type==SLICE_TYPE_P) {
    numRefIdx = shdr->num_ref_idx_l0_active;
  }
  else {
    numRefIdx = libde265_min(shdr->num_ref_idx_l0_active,
                             shdr->num_ref_idx_l1_active);
  }


  //int numInputMergeCand = *inout_numMergeCand;
  int zeroIdx = 0;

  while (*inout_numCurrMergeCand < maxCandidates) {
    // 1.

    logtrace(LogMotion,"zeroIdx:%d numRefIdx:%d\n", zeroIdx, numRefIdx);

    PBMotion* newCand = &out_mergeCandList[*inout_numCurrMergeCand];

    const int refIdx = (zeroIdx < numRefIdx) ? zeroIdx : 0;

    if (shdr->slice_type==SLICE_TYPE_P) {
      newCand->refIdx[0] = refIdx;
      newCand->refIdx[1] = -1;
      newCand->predFlag[0] = 1;
      newCand->predFlag[1] = 0;
    }
    else {
      newCand->refIdx[0] = refIdx;
      newCand->refIdx[1] = refIdx;
      newCand->predFlag[0] = 1;
      newCand->predFlag[1] = 1;
    }

    newCand->mv[0].x = 0;
    newCand->mv[0].y = 0;
    newCand->mv[1].x = 0;
    newCand->mv[1].y = 0;

    (*inout_numCurrMergeCand)++;

    // 2.

    zeroIdx++;
  }
}