void AppendTensorShapeToFingerprint(const PartialTensorShape& shape,
                                    Fprint128* fingerprint) {
  if (shape.unknown_rank()) {
    char c = '?';
    *fingerprint = FingerprintCat128(*fingerprint, c);
  } else {
    for (int i = 0; i < shape.dims(); i++) {
      int64_t dim = shape.dim_size(i);
      *fingerprint = FingerprintCat128(*fingerprint, dim);
    }
  }
}