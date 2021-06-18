#include <queue>
#include "utils.h"

static bool              status;
using namespace           utils;
typedef std::pair<int,int> edge;
typedef std::priority_queue<edge,std::vector<edge>,std::greater<>> PriorityQueue;

void set_adj_list(util * utl,std::vector<std::pair<int,int>> * adj) {
    int u = 1,v = 2; // point to current and next vertex
    int size = static_cast<int>(utl->flights.size());
    while (v < size) { // link origen and destiny that have same flight id
        std::string id_u = utl->flights[u].flight_id, id_v = utl->flights[v].flight_id;
        if (id_u == id_v && utl->flights[u].airport != utl->flights[v].airport) {
            tm in =  utl->flights[u].check_in, out = utl->flights[v].check_in;
            int w = utils::to_second(out) - utils::to_second(in);
            adj[u].emplace_back(v,w);
        }
        u = v++;
    }

    for (int index = 0; index < flight::airport_count; ++index) {// link vertex that have same airport id
        auto eq_range = utl->airports.equal_range(index);
        for (auto cur = eq_range.first;cur != eq_range.second;++cur) {
            if (cur->second.type != ARRIVAL) continue;
            for (auto nxt = eq_range.first;nxt != eq_range.second;++nxt) {
                if (nxt->second.type != DEPARTURE) continue;
                tm in = cur->second.check_in,out=nxt->second.check_in;
                int w;
                if (cur->second.flight_id == nxt->second.flight_id) {
                    w = utils::to_second(out) - utils::to_second(in);
                } else {
                    w = weight(in, out);
                }
                u = cur->second.index; v = nxt->second.index;
                adj[u].emplace_back(v,w);
            }
        }
    }
}

void dijkstra(std::vector<std::pair<int,int>> *adj,std::vector<int> &dist,std::vector<int> &parent) {
    PriorityQueue priorityQueue;
    priorityQueue.push({dist[0] = 0,0});
    while (!priorityQueue.empty()) {
        int u = priorityQueue.top().second;
        priorityQueue.pop();
        for (const auto &a:adj[u]) {
            int v = a.first;
            int weight = a.second + dist[u];
            if (dist[v] > weight) {
                parent[v] = u;
                priorityQueue.push({dist[v] = weight,v});
            }
        }
    }
}

void print_path(util * utl,std::vector<int>&parent,std::vector<int>&dist,int source,int destiny) {
    if (source == destiny) return;
    else if(destiny == -1)
        std::cout << ">> Impossible\n";
    else {
        print_path(utl,parent,dist,source,parent[destiny]);
        utils::flight &f_cur = utl->flights[destiny];
        utils::flight &f_prev = utl->flights[parent[destiny]];
        if(parent[destiny]) utils::to_print(f_prev,f_cur);
        if (f_cur.type == DEPARTURE)
            std::cout<<f_cur.flight_id<<' '<<utl->airport_names[f_cur.airport]<<' '<<std::asctime(&f_cur.check_in);
        else if (destiny != DEST)
            std::cout<<"\t\t"<<utl->airport_names[f_cur.airport]<<' '<<std::asctime(&f_cur.check_in);
    }
}

void set_origin(int u,bool opc,util * utl,std::vector<std::pair<int,int>> * adj,std::tm &in) {
    auto eq_range = utl->airports.equal_range(u);
    int check_in = utils::to_second(in),cur_check_in;
    bool find = false;
    std::vector<std::pair<int,int>> tmp;
    for (auto cur = eq_range.first;cur!=eq_range.second;++cur) {
        if (cur->second.type != DEPARTURE) continue;
        cur_check_in = utils::to_second(cur->second.check_in);
        if ((opc == CHECK_IN && cur_check_in == check_in) || (opc != CHECK_IN)) {
            find = true;
            adj[ORIG].emplace_back(cur->second.index,0);
        } else if (std::abs(cur_check_in - check_in) <= TOLERANCE) {
            tmp.emplace_back(cur->second.index,0);
        }
    }
    if (!find) adj[ORIG] = tmp;
}

void set_destiny(int u,bool opc,util * utl,std::vector<std::pair<int,int>> * adj,std::tm &out) {
    auto eq_range = utl->airports.equal_range(u);
    int check_out = utils::to_second(out),cur_check_out;
    bool find = false;
    std::vector<std::pair<int,int>> tmp;
    for (auto cur = eq_range.first;cur!=eq_range.second;++cur) {
        if (cur->second.type != ARRIVAL) continue;
        cur_check_out = utils::to_second(cur->second.check_in);
        if ((opc == CHECK_OUT && cur_check_out == check_out) || (opc != CHECK_OUT)) {
            find = true;
            adj[cur->second.index].emplace_back(DEST,0);
        } else if (std::abs(cur_check_out - check_out) <= TOLERANCE) {
            tmp.emplace_back(cur->second.index,0);
        }
    }
    for (int i = 0; i < tmp.size() && !find; ++i) {
        adj[tmp[i].first].emplace_back(DEST,0);
    }
}

void menu(util * utl,std::vector<std::pair<int,int>> *adj) {
    int opc,orig,dest;
    std::string arrival,departure,str_in,str_out;
    std::tm in{},out{};
    std::cout<<"\n>> Airport Departure: ";
    std::cin>>departure;
    std::cout<<">> Airport Arrival:   ";
    std::cin>>arrival;
    orig = utl->airport_id[departure];
    dest = utl->airport_id[arrival];
    utl->flights[ORIG].airport = orig;
    utl->flights[DEST].airport = dest;
    std::cout<<"[0] Continue\n"
               "[1] Enter to check-in\n"
               "[2] Enter to check-out\n"
               "[3] Enter to check-in and check-out\n >> ";
    std::cin>>opc;
    switch (opc) {
        case 0:
            set_origin(orig,!CHECK_IN,utl,adj,in);
            set_destiny(dest,!CHECK_OUT,utl,adj,out);
            break;
        case 1:
            std::cout<<">> Check-in (hh:mm): ";
            std::cin>>str_in;
            in = to_date(str_in);
            set_origin(orig,CHECK_IN,utl,adj,in);
            set_destiny(dest,CHECK_IN,utl,adj,out);
            break;
        case 2:
            std::cout<<">> Check-out (hh:mm): ";
            std::cin>>str_out;
            out = to_date(str_out);
            set_origin(orig,CHECK_OUT,utl,adj,in);
            set_destiny(dest,CHECK_OUT,utl,adj,out);
            break;
        case 3:
            std::cout<<">> Check-in (hh:mm): ";
            std::cin>>str_in;
            in = to_date(str_in);
            std::cout<<">> Check-out (hh:mm): ";
            std::cin>>str_out;
            out = to_date(str_out);
            set_origin(orig,CHECK_IN,utl,adj,in);
            set_destiny(dest,CHECK_OUT,utl,adj,out);
            break;
        default:
            std::cout<<"Invalid! Try again.\n";
            menu(utl,adj);
            break;
    }
}

int main() {
    util u;
    try {
        u = utils::set_up("grafo-01.txt");
    } catch (utils::exception&e) {
        std::cout<<e.what();
    }
    while (true) {
        std::vector<std::pair<int,int>> adj_list[flight::flight_count];
        set_adj_list(&u,adj_list);
        menu(&u,adj_list);
        std::vector<int> dist(flight::flight_count,INF);
        std::vector<int> parent(flight::flight_count,-1);
        dijkstra(adj_list,dist,parent);
        print_path(&u,parent,dist,ORIG,DEST);
        if (!IMPOSSIBLE)
            std::cout<<"Time: "<<dist[DEST]/MIN<<" min\n";
        std::cout<<"[0] Finish [1] Continue >> ";
        std::cin>>status;
        if (status) continue;
        return 0;
    }
}