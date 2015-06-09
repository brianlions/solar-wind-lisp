/*
 * file name:           include/random.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Mon Dec 08 17:58:06 2014 CST
 */

#ifndef _SOLAR_WIND_LISP_RANDOM_H_
#define _SOLAR_WIND_LISP_RANDOM_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace SolarWindLisp
{
namespace Utils
{

typedef class Prng
{
private:
    char statebuf_[64];
#ifdef __linux__
    struct random_data databuf_;
#endif

public:
    Prng(unsigned int seed = 1)
    {
        initialize(seed);
    }

    void initialize(unsigned int seed = 1)
    {
        memset(&statebuf_, 0, sizeof(statebuf_));
#ifdef __linux__
        memset(&databuf_, 0, sizeof(databuf_));
        initstate_r(seed, statebuf_, sizeof(statebuf_), &databuf_);
        setstate_r(statebuf_, &databuf_);
        srandom_r(seed, &databuf_);
#else
        initstate(seed, statebuf_, sizeof(statebuf_));
        setstate(statebuf_);
        srandom(seed);
#endif
    }

    int32_t random_i32()
    {
        return (int32_t) random_u32();
    }

    uint32_t random_u32()
    {
        uint32_t r, r2;
#ifdef __linux__
        // XXX random_r generate 31 bits integer
        random_r(&databuf_, (int32_t *) &r);
        random_r(&databuf_, (int32_t *) &r2);
#else
        r = random();
        r2 = random();
#endif
        return ((r << 19) | (r2 >> 12));
    }

    uint64_t random_u64()
    {
        uint32_t a = random_u32();
        uint32_t b = random_u32();
        uint64_t r = a;
        r <<= 32;
        r |= b;
        return r;
    }

    int64_t random_i64()
    {
        return (int64_t) random_u64();
    }
} PseudoRandomNumberGenerator;

} // namespace Utils
} // namespace SolarWindLisp

#endif /* _SOLAR_WIND_LISP_RANDOM_H_ */
