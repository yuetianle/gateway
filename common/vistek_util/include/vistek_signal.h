#ifndef vistek_signal_h__
#define vistek_signal_h__
#include <mutex>
#include <thread>
#include <future>

typedef boost::signals2::signal<void(void* handle, int errorcode, const char* msg, void* pUserData)> vsdk_message_signal;
typedef vsdk_message_signal::slot_function_type vsdk_message_callback;
#ifdef WIN32
typedef void(__cdecl * vsdk_message_callback_fp)(void* handle, int errorcode, const char* msg, void* pUserData);
#else
typedef void (*vsdk_message_callback_fp)(void* handle, int errorcode, const char* msg, void* pUserData);
#endif

struct vistek_condition
{
	std::mutex mtx;
	std::condition_variable cv;
};
typedef enum vistek_event_step
{
	ve_Begin,
	ve_Event,
	ve_End,
	ve_Timeout
};
class vistek_event_loop
{
	typedef boost::signals2::signal<int(vistek_event_loop*, vistek_event_step)> vistek_event_notify_signal;
	std::string m_event_name;
	bool m_is_shutdown;
	bool m_is_set;
	int m_timeout_ms;
	std::shared_ptr<std::thread> trd_event_loop;
	//vistek_condition m_cond;

	std::mutex mtx_event;
	std::condition_variable cond_event;

	vistek_event_notify_signal m_notify;
	std::mutex mtx_operation;
private:
	int proc();
public:
	vistek_event_loop(const std::string& name, int timeout_ms);
	~vistek_event_loop();
	std::string get_event_name() const;

	boost::signals2::connection start(vistek_event_notify_signal::slot_type cb);
	void stop();
	void set();
	void set_nolock();
	bool is_shutdown() const;
};
typedef std::shared_ptr<vistek_event_loop> vistek_event_loop_ptr;
template<class T>
class vistek_signal
{
	bool m_is_shutdown;
	bool m_is_set;
	std::string m_name;
	T m_ctx;
	std::mutex m_mtx;
	std::condition_variable m_cv;
public:
	vistek_signal(const std::string& name)
		: m_name(name), m_is_shutdown(false), m_is_set(false)
	{
		
	}

	~vistek_signal()
	{
		shutdown();
	}


	bool is_shutdown() const { return m_is_shutdown; }
	bool is_set() const { return m_is_set; }
	std::mutex& get_mtx() { return m_mtx; }
	T& get_ctx() { return m_ctx; }
	std::condition_variable& get_cv() { return m_cv; }
	void reset()
	{
		m_is_set = true;
		m_is_shutdown = false;
		m_cv.notify_all();
	}
	void shutdown()
	{
		m_is_set = true;
		m_is_shutdown = true;
		m_cv.notify_all();
	}
	//************************************
	// Method:    wait
	// FullName:  play_signal::wait
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: int millisecs if millisecs = -1 means wait infinitely
	//************************************
	bool wait(std::unique_lock<std::mutex>& lck, int millisecs)
	{
		std::cout << "signal: " << m_name << " begin to wait" << std::endl;
		do
		{
			if (millisecs > 0)
			{
				auto ret = m_cv.wait_for(lck, std::chrono::milliseconds(millisecs));
				if (ret != std::cv_status::no_timeout)
				{
					return false;
				}
			}
			else
			{
				m_cv.wait(lck);
				std::cout << "signal: " << m_name << " is waked up" << std::endl;
			}
		} while (!m_is_set);
		m_is_set = false;
		return true;
	}
	void set_signal()
	{
		m_is_set = true;
		m_cv.notify_all();
	}
};

#endif // vistek_signal_h__
