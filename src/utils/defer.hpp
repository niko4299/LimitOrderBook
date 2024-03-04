template <typename Function>
class Defer {
 public:
  explicit Defer(Function &&func) : _function{std::forward<Function>(func)} {}

  Defer(Defer const &) = delete;

  Defer(Defer &&) = delete;

  Defer &operator=(Defer const &) = delete;

  Defer &operator=(Defer &&) = delete;

  ~Defer() {
    _function();
  }

 private:
  std::function<void()> _function;
};
