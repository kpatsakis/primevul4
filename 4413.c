  const QuicServerConnectionState& getConn() const {
    return *dynamic_cast<QuicServerConnectionState*>(conn_.get());
  }