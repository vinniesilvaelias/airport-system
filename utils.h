//
// Created by vinicius on 14/06/2021.
//

#ifndef AIRPORT_SYSTEM_2_UTILS_H
#define AIRPORT_SYSTEM_2_UTILS_H
#include <iostream>
#include <fstream>
#include <ctime>
#include <map>
#include <utility>
#include <vector>
namespace utils {
#define ID          0
#define ORIGIN      1
#define IN          2
#define DESTINY     3
#define OUT         4
#define STOPOVER    5
#define INF         __INT_MAX__
#define DEST        flight::flight_count-1
#define ORIG        0
#define CHECK_IN    true
#define CHECK_OUT   false
#define TOLERANCE   30 * utils::MIN
#define IMPOSSIBLE  dist[DEST]/MIN == 35791394

    enum type_flight { DEPARTURE = true, ARRIVAL   = false };
    enum time        { MIN  = 60,   HOUR = 60,   DAY  = 24 };

    static std::map<int,std::string>     airport_names;
    static std::map<std::string,int>        airport_id;
    static std::vector<struct flight>          flights;
    static std::vector<std::string>          container;
    static std::multimap<int,flight>          airports;
    static std::map<int,flight>               m_flight;

    struct flight {
        static int                        flight_count;
        static int                       airport_count;
        std::string                          flight_id;
        int                                    airport;
        int                                      index;
        std::tm                               check_in;
        bool                                      type;
    };

    struct util {
        std::map<int,std::string>        airport_names;
        std::map<std::string,int>           airport_id;
        std::vector<flight>                    flights;
        std::multimap<int,flight>             airports;
    };

    struct exception : std::exception {
        std::string msg;
        explicit exception(std::string m) { msg = m; }
        const char * what() const noexcept override {return msg.data();};
    };

    util set_up(const std::string&);
    bool read_file(const std::string &);
    bool split(const std::string &,char);
    bool set_flight();
    void insert_airport(const std::string &);
    int to_second(tm &);
    std::tm to_date(const std::string &);
    int weight(std::tm &, std::tm &);
    void to_print(utils::flight &,utils::flight &);
}

#endif //AIRPORT_SYSTEM_2_UTILS_H