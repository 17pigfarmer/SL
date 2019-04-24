#pragma once

#include <stdint.h>

#include <atomic>

#include <windows.h>


#define ARCH_CPU_X86_FAMILY 1





#define LEVELDB_HAVE_MEMORY_BARRIER

// AtomicPointer built using platform-specific MemoryBarrier().
#if defined(LEVELDB_HAVE_MEMORY_BARRIER)
class AtomicPointer {
private:
	void* rep_;
public:
	AtomicPointer() { }
	explicit AtomicPointer(void* p) : rep_(p) {}
	inline void* NoBarrier_Load() const { return rep_; }
	inline void NoBarrier_Store(void* v) { rep_ = v; }
	inline void* Acquire_Load() const {
		void* result = rep_;
		MemoryBarrier();
		return result;
	}
	inline void Release_Store(void* v) {
		MemoryBarrier();
		rep_ = v;
	}
};

// AtomicPointer based on C++11 <atomic>.
#else
class AtomicPointer {
private:
	std::atomic<void*> rep_;
public:
	AtomicPointer() { }
	explicit AtomicPointer(void* v) : rep_(v) { }
	inline void* Acquire_Load() const {
		return rep_.load(std::memory_order_acquire);
	}
	inline void Release_Store(void* v) {
		rep_.store(v, std::memory_order_release);
	}
	inline void* NoBarrier_Load() const {
		return rep_.load(std::memory_order_relaxed);
	}
	inline void NoBarrier_Store(void* v) {
		rep_.store(v, std::memory_order_relaxed);
	}
};
#endif

#undef LEVELDB_HAVE_MEMORY_BARRIER
#undef ARCH_CPU_X86_FAMILY



