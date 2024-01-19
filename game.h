#pragma once

namespace Tmpl8
{
//forward declarations
class Tank;
class Rocket;
class Smoke;
class Particle_beam;
class Object_Pool;


class Game
{
    
  public:
    void set_target(Surface* surface) { screen = surface; }
    void init();
    void shutdown();
    void update(float deltaTime);
    void draw();
    void tick(float deltaTime);
    void insertion_sort_tanks_health(const std::vector<Tank>& original, std::vector<const Tank*>& sorted_tanks, int begin, int end);
    int get_health_from_ptr(Tank* ptr);
    void draw_health_bars(const std::vector<Tank*>& sorted_tanks, const int team);
    void measure_performance();
    void sort_nodes(KdTree::node** root, vector<KdTree::node*>* tobesortedchilderen);
    void nudge_and_collide_tank(KdTree::node* node, KdTree::node* rootblue, KdTree::node* rootred);
    void update_tank(KdTree::node* node, Terrain& background_terrain, ObjectPool<Rocket>& rocketpool);
    void update_particle_beam(vector<Particle_beam>& particle_beams, int index, vector<Tank>& tanks, vector<Smoke>& smokes);

    Tank& find_closest_enemy(Tank& current_tank);

    //new stuff
    __declspec(noinline) void init_tank_routes();
    __declspec(noinline) void nudge_and_collide_tanks();
    __declspec(noinline) void update_tanks();
    __declspec(noinline) void update_smoke();
    __declspec(noinline) void find_concave_hull();
    __declspec(noinline) void update_rockets();
    __declspec(noinline) void update_particle_beams();
    __declspec(noinline) void update_explosions();

    void mouse_up(int button)
    { /* implement if you want to detect mouse button presses */
    }

    void mouse_down(int button)
    { /* implement if you want to detect mouse button presses */
    }

    void mouse_move(int x, int y)
    { /* implement if you want to detect mouse movement */
    }

    void key_up(int key)
    { /* implement if you want to handle keys */
    }

    void key_down(int key)
    { /* implement if you want to handle keys */
    }

  private:
    Surface* screen;

    vector<Tank> tanks;
    vector<Tank*> tank_ptrs_sorted_on_health;
    vector<Smoke> smokes;
    vector<Explosion> explosions;
    vector<Particle_beam> particle_beams;
    

    Terrain background_terrain;
    std::vector<vec2> forcefield_hull;

    Font* frame_count_font;
    long long frame_count = 0;

    bool lock_update = false;

    //Checks if a point lies on the left of an arbitrary angled line
    bool left_of_line(vec2 line_start, vec2 line_end, vec2 point);
};

}; // namespace Tmpl8