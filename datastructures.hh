// Datastructures.hh
//
// Student name: Tuomas Mäkinen

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <functional>
#include <exception>
#include <set>
#include <list>
#include <queue>

// Types for IDs
using TownID = std::string;
using Name = std::string;

// Return values for cases where required thing was not found
TownID const NO_TOWNID = "----------";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "!!NO_NAME!!";

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Type for a distance (in metres)
using Distance = int;

// Return value for cases where Distance is unknown
Distance const NO_DISTANCE = NO_VALUE;

// This exception class is there just so that the user interface can notify
// about operations which are not (yet) implemented
class NotImplemented : public std::exception
{
public:
    NotImplemented() : msg_{} {}
    explicit NotImplemented(std::string const& msg) : msg_{msg + " not implemented"} {}

    virtual const char* what() const noexcept override
    {
        return msg_.c_str();
    }
private:
    std::string msg_;
};

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: ϴ(1)
    // Short rationale for estimate: size() is constant
    unsigned int town_count();

    // Estimate of performance: ϴ(n)
    // Short rationale for estimate: clear() is linear
    // in the size of the container.
    void clear_all();

    // Estimate of performance: ϴ(1) and O(n)
    // Short rationale for estimate: count() is
    // constant on average, worst case linear if the added town doesn't exist.
    bool add_town(TownID id, Name const& name, Coord coord, int tax);

    // Estimate of performance: ϴ(1) and O(n)
    // Short rationale for estimate: .at(key) and count() have the same performance
    Name get_town_name(TownID id);

    // Estimate of performance: ϴ(1) and O(n)
    // Short rationale for estimate: .at(key) and count() have the same performance
    Coord get_town_coordinates(TownID id);

    // Estimate of performance: ϴ(1) and O(n)
    // Short rationale for estimate: .at(key) and count() have the same performance
    int get_town_tax(TownID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: std::transform is linear due to
    // std::distance being linear
    std::vector<TownID> all_towns();

    // Estimate of performance: O(n)
    // Short rationale for estimate: For-loop time complexity
    // increases linearly with the size of the map
    std::vector<TownID> find_towns(Name const& name);

    // Estimate of performance: ϴ(1) and O(n)
    // Short rationale for estimate: count() is
    // constant on average, worst case linear if the town doesn't exist.
    bool change_town_name(TownID id, Name const& newname);

    // Estimate of performance: ϴ(nlog(n))
    // Short rationale for estimate: linear for-loop
    std::vector<TownID> towns_alphabetically();

    // Estimate of performance: ϴ(nlog(n))
    // Short rationale for estimate: linear for-loop
    std::vector<TownID> towns_distance_increasing();

    // Estimate of performance: ϴ(1)
    // Short rationale for estimate: town_count() is constant
    TownID min_distance();

    // Estimate of performance: ϴ(1)
    // Short rationale for estimate: town_count() is constant
    TownID max_distance();

    // Estimate of performance: ϴ(1) and O(n)
    // Short rationale for estimate: .at(key) and count() have the same performance
    bool add_vassalship(TownID vassalid, TownID masterid);

    // Estimate of performance: ϴ(1) and O(n)
    // Short rationale for estimate: count() is
    // constant on average, worst case linear if the town doesn't exist.
    std::vector<TownID> get_town_vassals(TownID id);

    // Estimate of performance: ϴ(n) and O(n^2)
    // Short rationale for estimate: recursive_tax_path() is called n times,
    // .at(key) worst case is linear.
    std::vector<TownID> taxer_path(TownID id);

    // Non-compulsory phase 1 operations

    // Estimate of performance: O(n^2)
    // Short rationale for estimate: For-loop time complexity increases linearly,
    // .at(key) worst case is linear.
    bool remove_town(TownID id);

    // Estimate of performance: ϴ(nlog(n))
    // Short rationale for estimate: set.insert is log(n) and
    // for-loop time complexity increases linearly
    std::vector<TownID> towns_nearest(Coord coord);

    // Estimate of performance: O(n^2)
    // Short rationale for estimate: recursive_longest_tax_path() is called n times and
    // .at(key) worst case is linear.
    std::vector<TownID> longest_vassal_path(TownID id);

    // Estimate of performance: O(n^2)
    // Short rationale for estimate: recursive_net_tax() is called n times and
    // .at(key) worst case is linear.
    int total_net_tax(TownID id);

    // Estimate of performance: O(n^2)
    // Short rationale for estimate: Linear for-loop and
    // clear() is linear in the size of the container.
    void clear_roads();

    // Estimate of performance: O(1)
    // Short rationale for estimate: Return is constant
    std::vector<std::pair<TownID, TownID>> all_roads();

    // Estimate of performance: O(n)
    // Short rationale for estimate: std::find() is
    // linear when the added road is a new one.
    bool add_road(TownID town1, TownID town2);

    // Estimate of performance: ϴ(1) and O(n)
    // Short rationale for estimate: find() is
    // constant on average, worst case linear if the town doesn't exist.
    std::vector<TownID> get_roads_from(TownID id);

    // Estimate of performance: O(n+k)
    // Short rationale for estimate: BFS is used and BFS'
    // time complexity is O(n+k)
    std::vector<TownID> any_route(TownID fromid, TownID toid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: std::find() in a vector
    // is a linear operation.
    bool remove_road(TownID town1, TownID town2);

    // Estimate of performance: O(n+k)
    // Short rationale for estimate: BFS is used and BFS'
    // time complexity is O(n+k)
    std::vector<TownID> least_towns_route(TownID fromid, TownID toid);

    // Estimate of performance: O(n+k)
    // Short rationale for estimate: DFS is used and DFS'
    // time complexity is O(n+k)
    std::vector<TownID> road_cycle_route(TownID startid);

private:

    struct town_data {
        Name name_;
        Coord coord_;
        int tax_;
        TownID vassalship_masterid = NO_TOWNID;
        std::vector<TownID> vassals;
        Distance distance_;
        bool visited = false;
        TownID reached_from_town = NO_TOWNID;
        std::vector<TownID> roads;

    };
    std::unordered_map<TownID, town_data> towns_;
    Distance calculate_distance(TownID, Coord);
    void update_min_max();

    TownID current_min;
    TownID current_max;
    Distance current_min_value = NO_DISTANCE;
    Distance current_max_value = NO_DISTANCE;

    void recursive_tax_path(TownID, std::vector<TownID> &v);
    void recursive_longest_tax_path(TownID id, int& longest, int current_depth, TownID &bottom_vassal);
    int recursive_net_tax(TownID);
    std::set<std::pair<Name, TownID>> names_;
    std::set<std::pair<Distance, TownID>> distances_;

    std::vector<std::pair<TownID, TownID>> vector_of_roads;
    std::vector<TownID> bfs(TownID town1, TownID town2);
    bool dfs(TownID town1, TownID parent, TownID start_town, std::vector<TownID>& v);
    void set_visited_false();


};

#endif // DATASTRUCTURES_HH
