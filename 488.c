  base::Time IncrementMockTime() {
    ++mock_time_counter_;
    return base::Time::FromDoubleT(mock_time_counter_ * 10.0);
  }
