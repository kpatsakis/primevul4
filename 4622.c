std::vector<float> GetBuckets(const int32_t feature,
                              const OpInputList& buckets_list) {
  const auto& buckets = buckets_list[feature].flat<float>();
  std::vector<float> buckets_vector(buckets.data(),
                                    buckets.data() + buckets.size());
  return buckets_vector;
}