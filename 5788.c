  std::size_t GetTupleBytes(const Tuple& tuple) {
    return std::accumulate(tuple.begin(), tuple.end(), 0,
                           [](const std::size_t& lhs, const Tensor& rhs) {
                             return lhs + rhs.TotalBytes();
                           });
  }