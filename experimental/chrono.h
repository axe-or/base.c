#pragma once

#include "prelude.h"

typedef i64 Time_Duration;

typedef struct {
	i64 sec;
	i64 nsec;
} Time_Spec;

#define Time_Nanosecond  ((i64)1)
#define Time_Microsecond ((i64)1000)
#define Time_Millisecond ((i64)1000 * (i64)1000)
#define Time_Second      ((i64)1000 * (i64)1000 * (i64)1000)
#define Time_Minute      ((i64)1000 * (i64)1000 * (i64)1000 * (i64)60)
#define Time_Hour        ((i64)1000 * (i64)1000 * (i64)1000 * (i64)60 * (i64)60)
#define Time_Day         ((i64)1000 * (i64)1000 * (i64)1000 * (i64)60 * (i64)60 * (i64)24)

void time_sleep(Time_Duration d);

Time_Spec time_now();

Time_Duration time_diff(Time_Spec a, Time_Spec b);

#ifdef BASE_C_IMPLEMENTATION

#ifdef __linux__
#include <time.h>
#include <unistd.h>

void time_sleep(Time_Duration d){
	struct timespec spec = {
		.tv_sec = d / 1000000000LL,
		.tv_nsec = d - (d / 1000000000LL),
	};

	struct timespec ignore;
	nanosleep(&spec, &ignore);
	(void)ignore;
}

Time_Spec time_now() {
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	return (Time_Spec){
		.sec = spec.tv_sec,
		.nsec = spec.tv_nsec,
	};
}

Time_Duration time_diff(Time_Spec a, Time_Spec b){
	Time_Duration da, db;
	da = a.sec * 1000000000LL + a.nsec;
	db = b.sec * 1000000000LL + b.nsec;
	return da - db;
}

#else
#error "Unimplemented platform"
#endif
#endif
