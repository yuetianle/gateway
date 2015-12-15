#ifndef vistek_perf_marker_h__
#define vistek_perf_marker_h__

#include <chrono>

#define vperf_begin(m, d) std::chrono::high_resolution_clock::time_point m = std::chrono::high_resolution_clock::now(); int64_t d = 0;
#ifdef _DEBUG
#define vperf_next(m, d, action) d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m).count(); \
												std::cout<<"####	action: "<<action<<" costs "<< d <<"ms"<<std::endl;	\
												m = std::chrono::high_resolution_clock::now();

#define vperf_end(m, d, action) d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m).count(); \
						std::cout<<"####	action: "<<action<<" costs "<< d <<"ms"<<std::endl;

#define vperf_next_micro(m, d, action) d = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m).count(); \
												std::cout<<"####	action: "<<action<<" costs "<< d <<"micro seconds"<<std::endl;	\
												m = std::chrono::high_resolution_clock::now();

#define vperf_end_micro(m, d, action) d = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m).count(); \
						std::cout<<"####	action: "<<action<<" costs "<< d <<"mics"<<std::endl;
#else
#define vperf_next(m, d, action) d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m).count(); \
												m = std::chrono::high_resolution_clock::now();																\
												std::cout<<"####	action: "<<action<<" costs "<< d <<"ms"<<std::endl;
#define vperf_end(m, d, action) d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m).count(); \
												std::cout<<"####	action: "<<action<<" costs "<< d <<"ms"<<std::endl;

#define vperf_next_micro(m, d, action) d = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m).count();\
												m = std::chrono::high_resolution_clock::now();																\
												std::cout<<"####	action: "<<action<<" costs "<< d <<"micro seconds"<<std::endl;	

#define vperf_end_micro(m, d, action) d = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m).count(); \
												std::cout<<"####	action: "<<action<<" costs "<< d <<"mics"<<std::endl;							
#endif
#endif // vistek_perf_marker_h__