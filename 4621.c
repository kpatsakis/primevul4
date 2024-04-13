std::vector<float> GenerateBoundaries(const QuantileStream& stream,
                                      const int64_t num_boundaries) {
  std::vector<float> boundaries = stream.GenerateBoundaries(num_boundaries);

  // Uniquify elements as we may get dupes.
  auto end_it = std::unique(boundaries.begin(), boundaries.end());
  boundaries.resize(std::distance(boundaries.begin(), end_it));
  return boundaries;
}