#ifndef vistek_lock_defs_h__
#define vistek_lock_defs_h__

#define USE_SPIN_LOCK
#include <mutex>

#define v_lock_v0(l, m) std::lock_guard<std::mutex> (l)((m))
#define v_lock_r_v0(l, m) std::lock_guard<std::recursive_mutex> (l)((m))

#define v_lock(l, m) std::lock_guard<decltype((m))> (l)((m))
#define v_lock_r(l, m) std::lock_guard<decltype((m))> (l)((m))

#define v_unique_lock_v0(l, m) std::unique_lock<std::mutex&> (l)((m))
#define v_unique_lock(l, m) std::unique_lock<decltype(m)> l(m)
#define v_ulock(l, m, t) std::unique_lock<decltype(m)> l(m, t)

#ifndef USE_SPIN_LOCK
#define vistek_smart_locker std::mutex
#define v_smart_lock(l, s) v_lock(l, m)
#else
#include <iostream>
#include <atomic>
class vistek_spin_lock
{
	typedef enum { vSpinLockLocked, vSpinLockUnlocked } v_spin_lock_state;
	std::atomic<v_spin_lock_state> m_state = ATOMIC_VAR_INIT(vSpinLockUnlocked);
public:
	vistek_spin_lock()
		: m_state(vSpinLockUnlocked)
	{

	}
	void lock()
	{
		while (m_state.exchange(vSpinLockLocked, std::memory_order_acquire) == vSpinLockLocked);
	}

	void unlock()
	{
		m_state.store(vSpinLockUnlocked, std::memory_order_release);
	}
};

class vistek_spin_lock_auto
{
	vistek_spin_lock &m_lock;
public:
	explicit vistek_spin_lock_auto(vistek_spin_lock &lock)
		: m_lock(lock)
	{
		m_lock.lock();
	}
	~vistek_spin_lock_auto()
	{
		m_lock.unlock();
	}
};
#define v_spin_lock(l, m) vistek_spin_lock_auto l(m)
#define vistek_smart_locker vistek_spin_lock
#define v_smart_lock(l, s) vistek_spin_lock_auto l(s)
#endif

#endif // vistek_lock_defs_h__