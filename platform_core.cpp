#include "platform_core.h"

// Platform Specific Code
#ifdef _WIN32
void gameSleep(int sleepMS) {
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepMS));
}

long getTimeInMillis() {
	std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
	auto now = std::chrono::system_clock::now().time_since_epoch();
	auto t100ms = std::chrono::milliseconds(100);
	auto time = now + t100ms;
    return static_cast<long>(std::chrono::duration_cast<std::chrono::milliseconds>(time).count());
}

#elif __EMSCRIPTEN__
void gameSleep(int sleepMS) {
		struct timespec ts;
		ts.tv_sec = (sleepMS) / 1000;
		ts.tv_nsec = ((sleepMS) % 1000) * 1000000;
		nanosleep(&ts, NULL);
}

long getTimeInMillis() {
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    return s*1000 + ms;
}
#elif __linux
void gameSleep(int sleepMS) {
		struct timespec ts;
		ts.tv_sec = (sleepMS) / 1000;
		ts.tv_nsec = ((sleepMS) % 1000) * 1000000;
		nanosleep(&ts, NULL);
}

long getTimeInMillis() {
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    return s*1000 + ms;
}
#elif ARDUINO
void gameSleep(int sleepMS) {
	delay(sleepMS);
}

long getTimeInMillis() {
	return millis();
}
#endif
