// Transport-independent, bounded half-duplex voice session for future audio adapters.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
namespace nabla::voice {
enum class State { idle, listening, waiting, speaking };
class Session {
 public:
  static constexpr size_t capacity = 3200; // 200 ms, mono 16 kHz signed PCM16.
  State state() const { return state_; }
  bool start() {
    if (!ready_ || muted_ || state_ != State::idle) return false;
    clear(); state_ = State::listening; return true;
  }
  void set_ready(bool ready) { ready_ = ready; if (!ready) cancel(); }
  void mute(bool muted) { muted_ = muted; if (muted) cancel(); }
  void cancel() { state_ = State::idle; clear(); }
  bool finish_capture() {
    if (state_ != State::listening || size_) return false;
    state_ = State::waiting; return true;
  }
  bool begin_response() {
    if (!ready_ || state_ != State::waiting) return false;
    state_ = State::speaking; return true;
  }
  bool finish_response() {
    if (state_ != State::speaking || size_) return false;
    cancel(); return true;
  }
  // Call from one owner task only. No allocations and no partial-frame enqueue.
  bool push(const int16_t *pcm, size_t count) {
    if (!ready_ || (state_ != State::listening && state_ != State::speaking) ||
        count > capacity-size_ || (!pcm && count)) return false;
    for (size_t i=0;i<count;i++) data_[(head_+size_+i)%capacity]=pcm[i];
    size_+=count; return true;
  }
  size_t pop(int16_t *pcm, size_t count) {
    if (!pcm) return 0;
    size_t n=count<size_?count:size_;
    for(size_t i=0;i<n;i++) {pcm[i]=data_[head_];data_[head_]=0;head_=(head_+1)%capacity;}
    size_-=n; return n;
  }
 private:
  void clear() { data_.fill(0); head_=size_=0; }
  std::array<int16_t,capacity> data_{};
  size_t head_{0},size_{0};
  bool ready_{false},muted_{false};
  State state_{State::idle};
};
} // namespace nabla::voice
