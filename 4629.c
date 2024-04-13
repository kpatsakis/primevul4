std::vector<float> GenerateQuantiles(const QuantileStream& stream,
                                     const int64_t num_quantiles) {
  // Do not de-dup boundaries. Exactly num_quantiles+1 boundary values
  // will be returned.
  std::vector<float> boundaries = stream.GenerateQuantiles(num_quantiles - 1);
  CHECK_EQ(boundaries.size(), num_quantiles);
  return boundaries;
}