// Datastructures.cc
//
// Student name: Tuomas Mäkinen

#include "datastructures.hh"

#include <random>

#include <cmath>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

Datastructures::Datastructures()
{

}

Datastructures::~Datastructures()
{

}

unsigned int Datastructures::town_count()
{
    return towns_.size();
}

void Datastructures::clear_all()
{
    towns_.clear();
    names_.clear();
    distances_.clear();
    vector_of_roads.clear();
}

bool Datastructures::add_town(TownID id, const Name &name, Coord coord, int tax)
{
    if (towns_.count(id) != 0) {
        return false;
    }
    town_data new_town;
    new_town.name_ = name;
    new_town.coord_ = coord;
    new_town.tax_ = tax;
    towns_.insert({id, new_town});
    int distance = std::floor(sqrt(pow(coord.x, 2) + pow(coord.y, 2)));
    new_town.distance_ = distance;

    if (current_min_value == NO_DISTANCE or distance < current_min_value) {
        current_min = id;
        current_min_value = distance;
    }
    if (current_max_value == NO_DISTANCE or distance > current_max_value) {
        current_max = id;
        current_max_value = distance;
    }
    names_.insert(std::make_pair(name, id));
    distances_.insert(std::make_pair(distance, id));
    return true;
}

Name Datastructures::get_town_name(TownID id)
{
    if (towns_.count(id) == 0) {
        return NO_NAME;
    }
    return towns_.at(id).name_;
}

Coord Datastructures::get_town_coordinates(TownID id)
{
    if (towns_.count(id) == 0) {
        return NO_COORD;
    }
    return towns_.at(id).coord_;
}

int Datastructures::get_town_tax(TownID id)
{
    if (towns_.count(id) == 0) {
        return NO_VALUE;
    }
    return towns_.at(id).tax_;
}

std::vector<TownID> Datastructures::all_towns()
{
    std::vector<TownID> all_towns_vec;
    std::transform(towns_.begin(), towns_.end(),
                   std::back_inserter(all_towns_vec),
                   [](const std::unordered_map<TownID, town_data>::value_type &pair){return pair.first;});
    return all_towns_vec;
}

std::vector<TownID> Datastructures::find_towns(const Name &name)
{
    std::vector<TownID> matching_towns;
    for (auto& town : towns_) {
        if (town.second.name_ == name)
        {
            matching_towns.push_back(town.first);
        }
    }
    return matching_towns;
}

bool Datastructures::change_town_name(TownID id, const Name &newname)
{
    if (towns_.count(id) == 0) {
        return false;
    }
    towns_.at(id).name_ = newname;
    return true;
}

std::vector<TownID> Datastructures::towns_alphabetically()
{
    std::vector<TownID> sorted;
    for (auto &i : names_) {
        sorted.push_back(i.second);
    }
    return sorted;
}

std::vector<TownID> Datastructures::towns_distance_increasing()
{
    std::vector<TownID> sorted;
    for (auto &i : distances_) {
        sorted.push_back(i.second);
    }
    return sorted;
}

TownID Datastructures::min_distance()
{
    if (town_count() == 0) {
        return NO_TOWNID;
    }
    return current_min;
}

TownID Datastructures::max_distance()
{
    if (town_count() == 0) {
        return NO_TOWNID;
    }
    return current_max;
}

bool Datastructures::add_vassalship(TownID vassalid, TownID masterid)
{
    if (towns_.count(vassalid) == 0 or towns_.count(masterid) == 0) { return false; }
    if (towns_.at(vassalid).vassalship_masterid != NO_TOWNID) { return false; }
    else if (masterid != vassalid) {
        towns_.at(vassalid).vassalship_masterid = masterid;
        towns_.at(masterid).vassals.push_back(vassalid);
        return true;
    }
    return false;
}

std::vector<TownID> Datastructures::get_town_vassals(TownID id)
{
    if (towns_.count(id) == 0) {return {NO_TOWNID};}

    return towns_.at(id).vassals;
}

std::vector<TownID> Datastructures::taxer_path(TownID id)
{
    if (towns_.count(id) == 0) {return {NO_TOWNID};}
    std::vector<TownID> path;
    path.push_back(id);
    recursive_tax_path(id, path);
    return path;
}

bool Datastructures::remove_town(TownID id)
{
    if (towns_.count(id) == 0) {return false;}
    if (towns_.at(id).vassalship_masterid != NO_TOWNID)
    {
        TownID master = towns_.at(id).vassalship_masterid;
        for (auto& i : towns_.at(id).vassals)
        {
            towns_.at(i).vassalship_masterid = master;
            towns_.at(master).vassals.push_back(i);
        }
        auto iter = std::find(towns_.at(master).vassals.begin(),
                              towns_.at(master).vassals.end(), id);
        towns_.at(master).vassals.erase(iter);
    }
    towns_.erase(id);
    auto iter = std::find_if(names_.begin(),
                          names_.end(), [id](const std::pair<Name, TownID>& p){ return p.second == id;});
    names_.erase(iter);
    auto iter2 = std::find_if(distances_.begin(),
                          distances_.end(), [id](const std::pair<Distance, TownID>& p){ return p.second == id;});
    distances_.erase(iter2);
    update_min_max();

    // Remove roads leading to deleted town
    for (auto& iter : towns_.at(id).roads) {
        remove_road(id, iter);
    }
    return true;
}

std::vector<TownID> Datastructures::towns_nearest(Coord coord)
{
    std::set<std::pair<Distance,TownID>> distances;
    for (auto &i : towns_) {
        Distance distance = calculate_distance(i.first, coord);
        distances.insert(std::make_pair(distance, i.first));
    }
    std::vector<TownID> sorted;
    for (auto &i : distances) {
        sorted.push_back(i.second);
    }
    return sorted;
}

std::vector<TownID> Datastructures::longest_vassal_path(TownID id)
{
    if (towns_.count(id) == 0) {return {NO_TOWNID};}

    std::vector<TownID> v;
    int depth = 0;
    TownID bottom_vassal = NO_TOWNID;

    recursive_longest_tax_path(id, depth, 1, bottom_vassal);
    v.push_back(bottom_vassal);
    recursive_tax_path(bottom_vassal, v);

    // Cutting the whole taxer path into the wanted one
    auto it = std::find(v.begin(), v.end(), id);
    std::vector<TownID> longest_path;
    std::copy(v.begin(), it, std::back_inserter(longest_path));
    longest_path.push_back(id);
    std::reverse(longest_path.begin(), longest_path.end());

    return longest_path;
}

int Datastructures::total_net_tax(TownID id)
{
    if (towns_.count(id) == 0) {return NO_VALUE;}
    if (towns_.at(id).vassalship_masterid == NO_TOWNID) {
        return recursive_net_tax(id);
    }
    else {
        int total = recursive_net_tax(id);
        return (total - std::floor(total * 0.1));
    }
}

Distance Datastructures::calculate_distance(TownID id, Coord coord)
{
    if (towns_.count(id) == 0) {return NO_DISTANCE;}
    Distance distance = std::floor(sqrt(pow(towns_.at(id).coord_.x - coord.x, 2)
                                               + pow(towns_.at(id).coord_.y - coord.y, 2)));
    return distance;
}

void Datastructures::update_min_max()
{
    current_min = distances_.begin()->second;
    current_min_value = distances_.begin()->first;
    current_max = distances_.rbegin()->second;
    current_max_value = distances_.rbegin()->first;
}

void Datastructures::recursive_tax_path(TownID id, std::vector<TownID> &v)
{
    TownID master = towns_.at(id).vassalship_masterid;
    if (master == NO_TOWNID) {
        return;
    }
    v.push_back(master);
    return recursive_tax_path(master, v);
}

void Datastructures::recursive_longest_tax_path(TownID id, int &longest, int current_depth, TownID &bottom_vassal)
{
    if (towns_.at(id).vassals.size() == 0) {
        if (current_depth > longest) {
            longest = current_depth;
            bottom_vassal = id;
        }
    }
    current_depth++;
    for (auto& vassal : towns_.at(id).vassals) {
        recursive_longest_tax_path(vassal, longest, current_depth, bottom_vassal);
    }
}

int Datastructures::recursive_net_tax(TownID id)
{
    if (towns_.at(id).vassals.size() == 0) {
        return towns_.at(id).tax_;
    }
    int vassal_tax = 0;
    for (auto& i : towns_.at(id).vassals) {
        vassal_tax += recursive_net_tax(i) * 0.1;
    }
    return vassal_tax + towns_.at(id).tax_;
}

void Datastructures::clear_roads()
{
    for (auto& i : towns_) {
        i.second.roads.clear();
    }
    vector_of_roads.clear();
}

std::vector<std::pair<TownID, TownID>> Datastructures::all_roads()
{
    return vector_of_roads;
}

bool Datastructures::add_road(TownID town1, TownID town2)
{
    if (towns_.count(town1) == 0 or towns_.count(town2) == 0) {return false;}
    if (std::find(towns_.at(town1).roads.begin(), towns_.at(town1).roads.end(), town2) != towns_.at(town1).roads.end()) {
        return false;
    }
    towns_.at(town1).roads.push_back(town2);
    towns_.at(town2).roads.push_back(town1);
    int x = town1.compare(town2);
    if (x > 0) {
        std::pair town_pair = std::make_pair(town2, town1);
        vector_of_roads.push_back(town_pair);
    }
    else {
        std::pair town_pair = std::make_pair(town1, town2);
        vector_of_roads.push_back(town_pair);
    }
    return true;
}

std::vector<TownID> Datastructures::get_roads_from(TownID id)
{
    auto search = towns_.find(id);
    if (search == towns_.end()) {
        return {NO_TOWNID};
    }
    return towns_.at(id).roads;
}

std::vector<TownID> Datastructures::any_route(TownID fromid, TownID toid)
{
    if (towns_.count(fromid) == 0 or towns_.count(toid) == 0) {return {NO_TOWNID};}
    set_visited_false();
    std::vector<TownID> path = bfs(fromid, toid);
    return path;
}

std::vector<TownID> Datastructures::bfs(TownID town1, TownID town2)
{
    if (town1 == town2) {
        return {town1};
    }
    std::vector<TownID> path;
    path.push_back(town1);
    std::deque<std::vector<TownID>> queue;
    queue.push_back(path);

    while (!queue.empty()) {
        path = queue.front();
        queue.pop_front();
        TownID last_node = *path.rbegin();

        if (last_node == town2) {
            return path;
        }
        for (auto& i : towns_.at(last_node).roads) {
            if (not towns_.at(i).visited) {
                towns_.at(i).visited = true;
                std::vector<TownID> new_path(path.begin(), path.end());
                new_path.push_back(i);
                queue.push_back(new_path);
            }
        }
    }
    return {};
}

bool Datastructures::dfs(TownID town1, TownID parent, TownID start_town, std::vector<TownID>& v)
{
    towns_.at(town1).visited = true;
    if (town1 != start_town) {
        towns_.at(town1).reached_from_town = parent;
    }
    for (auto& i : towns_.at(town1).roads) {
        if (not towns_.at(i).visited) {
            if (dfs(i, town1, start_town, v)) {
                v.push_back(i);
                return true;
            }
        }
        else if (i != parent) {
            v.push_back(i);
            return true;
        }
    }
    return false;
}


void Datastructures::set_visited_false()
{
    for (auto& i : towns_) {
        i.second.visited = false;
        i.second.reached_from_town = NO_TOWNID;
    }
}


bool Datastructures::remove_road(TownID town1, TownID town2)
{
    if (towns_.count(town1) == 0 or towns_.count(town2) == 0) {return false;}

    auto iter1 = std::find(towns_.at(town1).roads.begin(), towns_.at(town1).roads.end(), town2);
    if (iter1 != towns_.at(town1).roads.end()) {
        towns_.at(town1).roads.erase(iter1);
        auto iter2 = std::find(towns_.at(town2).roads.begin(), towns_.at(town2).roads.end(), town1);
        towns_.at(town2).roads.erase(iter2);

        // Finding correct pair to delete
        int x = town1.compare(town2);
        if (x > 0) {
            auto pair = std::make_pair(town2, town1);
            auto i = std::find(vector_of_roads.begin(), vector_of_roads.end(), pair);
            vector_of_roads.erase(i);
        }
        else {
            auto pair = std::make_pair(town1, town2);
            auto i = std::find(vector_of_roads.begin(), vector_of_roads.end(), pair);
            vector_of_roads.erase(i);
        }
        return true;
    }
    else {
        return false;
    }
}

std::vector<TownID> Datastructures::least_towns_route(TownID fromid, TownID toid)
{
    if (towns_.count(fromid) == 0 or towns_.count(toid) == 0) {return {NO_TOWNID};}
    set_visited_false();
    std::vector<TownID> path = bfs(fromid, toid);
    return path;
}

std::vector<TownID> Datastructures::road_cycle_route(TownID startid)
{
    if (towns_.count(startid) == 0) {return {NO_TOWNID};}
    set_visited_false();

    std::vector<TownID> path;
    dfs(startid, NO_TOWNID, startid, path);
    if (path.empty()) {return {};}
    path.push_back(startid);
    std::reverse(path.begin(), path.end());
    return path;
}
