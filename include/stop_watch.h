/*
 * file name:           include/stop_watch.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Thu Jan 22 17:50:41 2015 CST
 */

#ifndef _SOLAR_WIND_LISP_STOP_WATCH_H_
#define _SOLAR_WIND_LISP_STOP_WATCH_H_

#include <sys/time.h>
#include <stdint.h>

namespace SolarWindLisp
{

class StopWatch
{
public:
    StopWatch()
    {
        reset();
    }

    void reset()
    {
        _status = NONE;
        timerclear(&_start_time);
        timerclear(&_stop_time);
    }

    void restart()
    {
        gettimeofday(&_start_time, NULL);
        _status = STARTED;
    }

    bool start()
    {
        if (_status == NONE) {
            gettimeofday(&_start_time, NULL);
            _status = STARTED;
            return true;
        }

        return false;
    }

    bool stop()
    {
        if (_status == STARTED) {
            gettimeofday(&_stop_time, NULL);
            _status = STOPPED;
            return true;
        }

        return false;
    }

    uint64_t timecost_usec() const
    {
        struct timeval now;
        switch (_status) {
            case STOPPED:
                return static_cast<uint64_t>(difference_usec(&_stop_time, &_start_time));
            case STARTED:
                gettimeofday(&now, NULL);
                return static_cast<uint64_t>(difference_usec(&now, &_start_time));
            default:
                return 0;
        }
    }

    uint64_t timecost_msec() const
    {
        struct timeval now;
        switch (_status) {
            case STOPPED:
                return static_cast<uint64_t>(difference_msec(&_stop_time, &_start_time));
            case STARTED:
                gettimeofday(&now, NULL);
                return static_cast<uint64_t>(difference_msec(&now, &_start_time));
            default:
                return 0;
        }
    }

    double timecost() const
    {
        struct timeval now;
        switch (_status) {
            case STOPPED:
                return difference(&_stop_time, &_start_time);
            case STARTED:
                gettimeofday(&now, NULL);
                return difference(&now, &_start_time);
            default:
                return 0;
        }
    }

    double start_time() const
    {
        return _status == NONE ? 0 : epoch_time(&_start_time);
    }

    double stop_time() const
    {
        return _status == STOPPED ? epoch_time(&_stop_time) : 0;
    }

    static int64_t difference_usec(const struct timeval *ta, const struct timeval *tb)
    {
        return static_cast<int64_t>(ta->tv_sec - tb->tv_sec) * 1000000 + (ta->tv_usec - tb->tv_usec);
    }

    static int64_t difference_msec(const struct timeval *ta, const struct timeval *tb)
    {
        return static_cast<int64_t>(ta->tv_sec - tb->tv_sec) * 1000 + (ta->tv_usec - tb->tv_usec) / 1000;
    }

    static double difference(const struct timeval *ta, const struct timeval *tb)
    {
        return 0.000001 * (ta->tv_usec - tb->tv_usec) + (ta->tv_sec - tb->tv_sec);
    }

    static double epoch_time(const struct timeval *ta)
    {
        return timerisset(ta) ? (0.000001 * ta->tv_usec + ta->tv_sec) : 0;
    }

private:
    enum Status
    {
        NONE = 0,
        STARTED = 1,
        STOPPED = 2
    };

    struct timeval _start_time;
    struct timeval _stop_time;
    Status _status;
};

} // name SolarWindLisp

#endif // _SOLAR_WIND_LISP_STOP_WATCH_H_

