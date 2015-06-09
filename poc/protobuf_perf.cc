#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include "stop_watch.h"
#include "random.h"
#include "gnu_attributes.h"
#include "planetary_system.pb.h"

#define array_size(a) sizeof(a) / sizeof(a[0])

struct Star {
    const char * name;
    double radius_km;
    double mass_kg;
    double age_billion_years;
} stars[] = {
    { "sun", 696342, 1.98855e30, 4.6 }
};

struct Moon {
};

struct Planet {
    const char * name;
    double radius_km;
    double mass_kg;
    double semi_major_axis_au;
    double age_billion_years;
} planets[] = {
    { "mercury", 2439.7, 3.3022e23,  0.387098,   0 },
    { "venus",   6051.8, 1.8676e24,  0.723327,   0 },
    { "earth",   6371.0, 5.97219e24, 1.00000011, 0 },
    { "mars",    3389.5, 6.4185e23,  1.523679,   0 },
    { "jupiter", 69911,  1.8986e27,  5.204267,   0 },
    { "saturn",  58232,  5.6846e26,  9.5820172,  0 },
    { "uranus",  25362,  8.6810e25, 19.189253,   0 },
    { "neptune", 24622,  1.0243e26, 30.070900,   0 },
};

void init_planetary_system(pbperftest::PlanetarySystem * a_system, int copies)
{
    a_system->set_name("solar system");
    for (size_t i = 0; i < array_size(stars); ++i) {
        pbperftest::PlanetarySystem::Star * star = a_system->add_star();
        star->set_name(stars[i].name);
        star->set_radius_km(stars[i].radius_km);
        star->set_mass_kg(stars[i].mass_kg);
        star->set_age_billion_years(stars[i].age_billion_years);
    }

    // outer loop: add multiple copies of every planet, to make `a_system' is large enough
    for (size_t j = 0; j < static_cast<size_t>(copies); ++j) {
        for (size_t i = 0; i < array_size(planets); ++i) {
            pbperftest::PlanetarySystem::Planet * planet = a_system->add_planet();
            planet->set_name(planets[i].name);
            planet->set_radius_km(planets[i].radius_km);
            planet->set_mass_kg(planets[i].mass_kg);
            planet->set_semi_major_axis_au(planets[i].semi_major_axis_au);
            planet->set_age_billion_years(planets[i].age_billion_years);
        }
    }

    std::cout
        << "star_size(): "   << a_system->star_size() << std::endl
        << "planet_size(): " << a_system->planet_size() << std::endl
        << "ByteSize(): " << a_system->ByteSize() << std::endl;
}

void check_before_get(const pbperftest::PlanetarySystem::Planet &planet,
        int iterations, bool print)
{
    std::string name;
    double val;
    int counter = 0;

    SolarWindLisp::StopWatch sw;
    sw.start();
    for (int i = 0; i < iterations; ++i) {
        if (planet.has_name()) {
            name = planet.name();
            counter++;
        }

        if (planet.has_radius_km()) {
            val = planet.radius_km();
            counter++;
        }

        if (planet.has_mass_kg()) {
            val = planet.mass_kg();
            counter++;
        }

        if (planet.has_semi_major_axis_au()) {
            val = planet.semi_major_axis_au();
            counter++;
        }

        if (planet.has_age_billion_years()) {
            val = planet.age_billion_years();
            counter++;
        }
    }
    sw.stop();

    double avg = 1.0 * sw.timecost_usec() / iterations;
    if (counter != iterations * 5) {
        std::cerr << "[+] oops, code probably ignored by compiler!" << std::endl;
    }
    else if (print) {
        std::cout
            << "check_before_get(): "
            //<< "  " << planet.ShortDebugString() << std::endl
            << "  iterations " << iterations
            << "  avg time cost " << avg << " usec"
            << "  (ignore this line " << name << "," << val << ")" << std::endl;
    }
}

void usage(const char * prog)
{
    fprintf(stderr, "usage: %s [copies [outer_iter [inner_iter]]]\n", prog);
    exit(EXIT_FAILURE);
}

int main(int argc, char ** argv)
{
    int copies     = 1000;
    int outer_iter = 1000;
    int inner_iter = 1000;
    if ((argc > 4)
            || (argc == 1)
            || (argc > 3 && (inner_iter = atoi(argv[3])) <= 0)
            || (argc > 2 && (outer_iter = atoi(argv[2])) <= 0)
            || (argc > 1 && (copies = atoi(argv[1])) <= 0)) {
        usage(argv[0]);
    }

    pbperftest::PlanetarySystem a_system;
    init_planetary_system(&a_system, copies);

    SolarWindLisp::Utils::Prng prng;
    SolarWindLisp::StopWatch stop_watch;

    int planet_size = a_system.planet_size();


    std::cout
        << "copies:      " << copies << std::endl
        << "outer_iter:  " << outer_iter << std::endl
        << "planet_size: " << planet_size << std::endl
        << "inner_iter:  " << inner_iter << std::endl;

    pbperftest::PlanetarySystem::Planet planet;
    stop_watch.start();
    for (int j = 0; j < outer_iter; ++j) {
        for (int i = 0; i < planet_size; ++i) {
            check_before_get(a_system.planet(prng.random_u32() % planet_size), inner_iter, false);
        }
    }
    stop_watch.stop();

    int64_t counts = 1.0 * outer_iter * inner_iter * planet_size;
    double avg = 1.0 * stop_watch.timecost_usec() / counts;
    std::cout
        << "total time cost " << stop_watch.timecost_usec() << " usec" << std::endl
        << "avg pb access time cost " << avg << " usec" << std::endl;

    exit(EXIT_SUCCESS);
}
