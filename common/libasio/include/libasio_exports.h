#ifndef libasio_exports_h__
#define libasio_exports_h__

#ifdef _WIN32 //windows

#ifdef LIBASIO_EXPORTS
#define LIBASIO_API __declspec(dllexport)
#else
#define LIBASIO_API __declspec(dllimport)
#endif
#else
#define LIBASIO_API 
#endif
#endif // libasio_exports_h__