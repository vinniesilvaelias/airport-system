//
// Created by vinicius on 14/06/2021.
//

#include "utils.h"

namespace utils {

    int flight::airport_count = 0, flight::flight_count  = 0;

    utils::util set_up(const std::string & name_file) {
        flight f = {"",0,flight::flight_count++,{},DEPARTURE};
        utils::flights.push_back(f);

        if (!read_file(name_file))  throw utils::exception ("Could not read file!\n");

        f = {"",0,flight::flight_count++,{},ARRIVAL};
        utils::flights.push_back(f);

        return {utils::airport_names,airport_id,flights,airports};
    }

    bool read_file(const std::string & name_file) {
        if (name_file.empty()) return false;
        std::fstream input_file(name_file,std::ios::in);
        if (!input_file) return false;
        std::string line;
        std::getline(input_file,line);
        while (!line.empty())
            if (split(line, ' ') && set_flight()) std::getline(input_file, line);
            else { input_file.close(); throw utils::exception("Split fail!\n"); }
        input_file.close();
        return true;
    }

    bool split(const std::string & str,char ch) {
        if (str.empty()) return false;
        utils::container.clear();
        size_t begin = 0, last;
        last = str.find_first_of(ch);
        while (last != std::string::npos) {
            utils::container.push_back(str.substr(begin,last - begin));
            begin = ++last;
            last = str.find_first_of(' ', begin);
        }
        if (utils::container.empty()) return false;
        utils::container.push_back(str.substr(begin,last-begin));
        return true;
    }

    bool set_flight() {
        if (utils::container.empty()) return false;

        insert_airport(utils::container[ORIGIN]);
        insert_airport(utils::container[DESTINY]);

        flight f1 = {utils::container[ID],utils::airport_id[utils::container[ORIGIN]],
                     flight::flight_count++,to_date(utils::container[IN]),DEPARTURE };


        flight f2 = {utils::container[ID],utils::airport_id[utils::container[DESTINY]],
                     flight::flight_count++,to_date(utils::container[OUT]),ARRIVAL };

        utils::flights.push_back(f1);
        utils::flights.push_back(f2);

        airports.insert({f1.airport,f1});
        airports.insert({f2.airport,f2});

        if (utils::container.size() >= STOPOVER) {
            auto it = utils::container.begin() + STOPOVER;

            while (it != utils::container.end()) {
                f1 = f2;
                f2.check_in = to_date(*(it)++);
                f2.index = flight::flight_count++;
                f2.type = !f2.type;

                utils::flights.push_back(f2);

                airports.insert({f2.airport,f2});

                insert_airport(*(it));

                f1.index = utils::flight::flight_count++;
                f1.airport = utils::airport_id[*(it)++];
                f1.check_in = to_date(*(it)++);
                f1.type = ARRIVAL;

                utils::flights.push_back(f1);
                airports.insert({f1.airport,f1});
            }
        }
        return true;
    }

    void insert_airport(const std::string &name) {
        std::pair<std::map<std::string,int>::iterator,bool> ret;
        ret = utils::airport_id.insert({name,flight::airport_count});
        if (ret.second) utils::airport_names.insert({flight::airport_count++,name});
    }

    std::tm to_date(const std::string & time) {
        time_t now;
        std::time(&now);
        std::tm date = *(localtime(&now));
        date.tm_min  = stoi(time.substr(3,2));
        date.tm_hour = stoi(time.substr(0,2));
        date.tm_sec  = 0;
        return date;
    }

    int to_second(tm & time) { return static_cast<int>(mktime(&time)); }

    int weight(std::tm & in,std::tm & out) {
        int w = to_second(out) - to_second(in);
        return w < 30 * MIN ? w + MIN * HOUR * DAY : w;
    }

    void to_print(flight &f_prev,flight &f_cur) {
        int cur_sec = to_second(f_cur.check_in);
        int prev_sec = to_second(f_prev.check_in);
        if (cur_sec - prev_sec < 0) {
            cur_sec += MIN*HOUR*DAY;
            std::time_t t_cur = cur_sec;
            f_cur.check_in = *(std::localtime(&t_cur));
        }
    }
}