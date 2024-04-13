  QuicServerConnectionState& getNonConstConn() {
    return *dynamic_cast<QuicServerConnectionState*>(conn_.get());
  }