#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
namespace zs { namespace transfer_profile {
enum class Direction : unsigned { HtoD=0, DtoH=1, DtoD=2, Generic=3, Count=4 };
struct Snapshot {
 std::uint64_t h_to_d_bytes=0, d_to_h_bytes=0, d_to_d_bytes=0;
 std::uint64_t h_to_d_calls=0, d_to_h_calls=0, d_to_d_calls=0;
 std::uint64_t generic_bytes=0, generic_calls=0;
};
struct Counters {
 std::atomic<std::uint64_t> bytes[4]{};
 std::atomic<std::uint64_t> calls[4]{};
};
inline Counters &counters() noexcept { static Counters value{}; return value; }
inline void record(Direction direction,std::size_t bytes) noexcept {
 const auto i=static_cast<unsigned>(direction);
 if(i>=static_cast<unsigned>(Direction::Count)) return;
 counters().bytes[i].fetch_add(static_cast<std::uint64_t>(bytes),std::memory_order_relaxed);
 counters().calls[i].fetch_add(1,std::memory_order_relaxed);
}
inline Snapshot snapshot() noexcept {
 auto &c=counters(); Snapshot s;
 s.h_to_d_bytes=c.bytes[0].load(std::memory_order_relaxed);
 s.d_to_h_bytes=c.bytes[1].load(std::memory_order_relaxed);
 s.d_to_d_bytes=c.bytes[2].load(std::memory_order_relaxed);
 s.generic_bytes=c.bytes[3].load(std::memory_order_relaxed);
 s.h_to_d_calls=c.calls[0].load(std::memory_order_relaxed);
 s.d_to_h_calls=c.calls[1].load(std::memory_order_relaxed);
 s.d_to_d_calls=c.calls[2].load(std::memory_order_relaxed);
 s.generic_calls=c.calls[3].load(std::memory_order_relaxed);
 return s;
}
inline void reset() noexcept {
 auto &c=counters();
 for(unsigned i=0;i!=4;++i){ c.bytes[i].store(0,std::memory_order_relaxed); c.calls[i].store(0,std::memory_order_relaxed); }
}
} } // namespace zs::transfer_profile
