inline tensorflow::Fprint128 FingerprintCat128(const tensorflow::Fprint128& a,
                                               const int64_t b) {
  auto x = tensorflow::FingerprintCat64(a.low64, b);
  return {x, tensorflow::FingerprintCat64(a.high64, x)};
}