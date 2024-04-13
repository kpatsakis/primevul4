  void deliverDataWithoutErrorCheck(
      Buf data,
      bool writes = true,
      folly::SocketAddress* peer = nullptr) {
    data->coalesce();
    server->onNetworkData(
        peer == nullptr ? clientAddr : *peer,
        NetworkData(std::move(data), Clock::now()));
    if (writes) {
      loopForWrites();
    }
  }