  TestingQuicServerTransport(
      folly::EventBase* evb,
      std::unique_ptr<folly::AsyncUDPSocket> sock,
      ConnectionCallback& cb,
      std::shared_ptr<const fizz::server::FizzServerContext> ctx)
      : QuicServerTransport(evb, std::move(sock), cb, ctx) {}