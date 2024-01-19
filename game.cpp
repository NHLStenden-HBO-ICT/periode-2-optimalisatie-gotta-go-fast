#include "precomp.h" // include (only) this in every .cpp file


constexpr auto num_tanks_blue = 2048;
constexpr auto num_tanks_red = 2048;
constexpr auto num_tanks = num_tanks_blue + num_tanks_red;

constexpr auto tank_max_health = 1000;
constexpr auto rocket_hit_value = 60;
constexpr auto rocket_speed_multiplier = 3;
constexpr auto particle_beam_hit_value = 50;

constexpr auto tank_max_speed = 1.0;

constexpr auto health_bar_width = 70;

constexpr auto max_frames = 2000;

//Global performance timer
constexpr auto REF_PERFORMANCE = 57344.9; //UPDATE THIS WITH YOUR REFERENCE PERFORMANCE (see console after 2k frames)
static timer perf_timer;
static float duration;

//Load sprite files and initialize sprites
static Surface* tank_red_img = new Surface("assets/Tank_Proj2.png");
static Surface* tank_blue_img = new Surface("assets/Tank_Blue_Proj2.png");
static Surface* rocket_red_img = new Surface("assets/Rocket_Proj2.png");
static Surface* rocket_blue_img = new Surface("assets/Rocket_Blue_Proj2.png");
static Surface* particle_beam_img = new Surface("assets/Particle_Beam.png");
static Surface* smoke_img = new Surface("assets/Smoke.png");
static Surface* explosion_img = new Surface("assets/Explosion.png");

static Sprite tank_red(tank_red_img, 12);
static Sprite tank_blue(tank_blue_img, 12);
static Sprite rocket_red(rocket_red_img, 12);
static Sprite rocket_blue(rocket_blue_img, 12);
static Sprite smoke(smoke_img, 4);
static Sprite explosion(explosion_img, 9);
static Sprite particle_beam_sprite(particle_beam_img, 3);

const static vec2 tank_size(7, 9);
const static vec2 rocket_size(6, 6);

const static float tank_radius = 3.f;
const static float rocket_radius = 5.f;

ObjectPool<Rocket> rockets_pool = ObjectPool<Rocket>{ num_tanks * 2,
    Rocket {
        vec2{0, 0},
        vec2{1, 1},
        1.0f,
        RED,
        &rocket_red
} };

KdTree::node* root_Blue;
KdTree::node* root_Red;

vector<KdTree::node*> to_be_sorted_childeren_blue;
vector<KdTree::node*> to_be_sorted_childeren_red;
vector<KdTree::node*> to_be_sorted_childeren;
KdTree tree;
static ThreadPool threadpool = ThreadPool(std::thread::hardware_concurrency());

/// <summary>
/// initializing game
/// </summary>
void Game::init()
{
    tree = KdTree();
    frame_count_font = new Font("assets/digital_small.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZ:?!=-0123456789.");

    tanks.reserve(num_tanks_blue + num_tanks_red);
    smokes.reserve(num_tanks_blue + num_tanks_red);

    uint max_rows = 24;

    float start_blue_x = tank_size.x + 40.0f;
    float start_blue_y = tank_size.y + 30.0f;

    float start_red_x = 1088.0f;
    float start_red_y = tank_size.y + 30.0f;

    float spacing = 7.5f;


    //Spawn blue tanks
    for (int i = 0; i < num_tanks_blue; i++)
    {
        vec2 position{ start_blue_x + ((i % max_rows) * spacing), start_blue_y + ((i / max_rows) * spacing) };
        tanks.push_back(Tank(position.x, position.y, BLUE, &tank_blue, &smoke, 1100.f, position.y + 16, tank_radius, tank_max_health, tank_max_speed));

    }
    //Spawn red tanks
    for (int i = 0; i < num_tanks_red; i++)
    {
        vec2 position{ start_red_x + ((i % max_rows) * spacing), start_red_y + ((i / max_rows) * spacing) };
        tanks.push_back(Tank(position.x, position.y, RED, &tank_red, &smoke, 100.f, position.y + 16, tank_radius, tank_max_health, tank_max_speed));
    }


    for (Tank& tank: tanks) {
        tank_ptrs_sorted_on_health.push_back(&tank);
    }

    particle_beams.push_back(Particle_beam(vec2(590, 327), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));
    particle_beams.push_back(Particle_beam(vec2(64, 64), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));
    particle_beams.push_back(Particle_beam(vec2(1200, 600), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));

    //initialise tree first get root tank aka middlest tank 

    //if one tree no 2 lists.
    vector<KdTree::node*> bluelist;
    vector<KdTree::node*> redlist;

    for (int y = 0; y < tanks.size(); y++ ) {
        if (tanks[y].allignment == BLUE) {
           bluelist.push_back(tree.newnode(&tanks[y]));
        }
        if (tanks[y].allignment == RED) { 
            redlist.push_back(tree.newnode(&tanks[y]));
        }
    }


    root_Blue = tree.insert_nodes(bluelist,0);
    root_Red = tree.insert_nodes(redlist, 0);

}

/// <summary>
/// Close down application
/// </summary>
void Game::shutdown()
{
}

/// <summary>
/// Iterates through all tanks and returns the closest enemy tank for the given tank
/// </summary>
/// <param name="current_tank">tank of which closest needs to be found</param>
/// <returns>the closest enemy</returns>
Tank& Game::find_closest_enemy(Tank& current_tank)
{
    if (current_tank.allignment == RED) {
        return *tree.search_closest(root_Blue, current_tank.position,0)->tank;
    }    
    else if (current_tank.allignment == BLUE) {
        return *tree.search_closest(root_Red, current_tank.position,0)->tank;
    }
}

/// <summary>
/// Checks if a point lies on the left of an arbitrary angled line
/// </summary>
/// <param name="line_start">start of arbitrary angled line</param>
/// <param name="line_end">end of arbitrary angled line</param>
/// <param name="point">the point which is checked</param>
/// <returns>a bool gives true if point lies on the left of an arbitrary angled line</returns>
bool Tmpl8::Game::left_of_line(vec2 line_start, vec2 line_end, vec2 point)
{
    return ((line_end.x - line_start.x) * (point.y - line_start.y) - (line_end.y - line_start.y) * (point.x - line_start.x)) < 0;
}

/// <summary>
/// Calculate the route to the destination for each tank using BFS
/// </summary>
void Game::init_tank_routes() {
    for (Tank& t : tanks)
    {
        t.set_route(background_terrain.get_route(t, t.target));
    }
}

/// <summary>
/// Main nudge and collide function that checks for each node.
/// Add for each node a task to threadpool and put the future of that in a list
/// Main thread can only continue when all threads are finished.
/// </summary>
void Game::nudge_and_collide_tanks() {

    vector<KdTree::node*> nodes = *tree.get_tobe_sortedlist(root_Blue, &to_be_sorted_childeren, 0);
    nodes = *tree.get_tobe_sortedlist(root_Red, &nodes, 0);


    vector<future<void>> futures_list;
    futures_list.reserve(num_tanks);
    for (KdTree::node* node : nodes) 
    {
        futures_list.emplace_back(threadpool.enqueue([=] {nudge_and_collide_tank(node, root_Blue, root_Red); }));
    }
    for (future<void>& future : futures_list) { 
        future.wait();
    }

    to_be_sorted_childeren.erase(to_be_sorted_childeren.begin(), to_be_sorted_childeren.end());
}

/// <summary>
/// The function that calculates the colides and nudges of the tanks.
/// It first gathers all active tanks in a list based on the two trees.
/// It uses the kd tree to see which is closest of the red and blue tree each and then compares the two to see which one is truly the closest.
/// It then calculates wether a nudge must happen or not.
/// </summary>
/// <param name="node">node which contains a tank of which the closests needs to be found</param>
/// <param name="rootblue">the root of the blue tree</param>
/// <param name="rootred">the root of the blue tree<</param>
void Tmpl8::Game::nudge_and_collide_tank(KdTree::node* node, KdTree::node* rootblue, KdTree::node* rootred)
{
    KdTree::node* checkblue = tree.search_closest_other_tank(root_Blue, node->tank, 0);
    KdTree::node* checkred = tree.search_closest_other_tank(root_Red, node->tank, 0);
    KdTree::node* closest = tree.get_closest(node->tank, checkblue, checkred);

    vec2 dir = node->tank->get_position() - closest->tank->get_position();
    float dir_squared_len = dir.sqr_length();

    float col_squared_len = (node->tank->get_collision_radius() + closest->tank->get_collision_radius());
    col_squared_len *= col_squared_len;

    if (dir_squared_len < col_squared_len)
    {
        node->tank->push(dir.normalized(), 1.f);
    }
}

/// <summary>
/// Update tanks function
/// </summary>
void Game::update_tanks() {

    for (Tank& tank : tanks)
    {
        if (tank.active)
        {
            //Move tanks according to speed and nudges (see above) also reload
            tank.tick(background_terrain);

            //Shoot at closest target if reloaded
            if (tank.rocket_reloaded())
            {
                Tank& target = find_closest_enemy(tank);

                //cout << "instancing rocket" << endl;
                Rocket* rocket = rockets_pool.get();

                rocket->active = true;
                rocket->allignment = tank.allignment;
                rocket->collision_radius = rocket_radius;
                rocket->current_frame = 0;
                rocket->position = tank.position;
                rocket->speed = (target.get_position() - tank.position).normalized() * rocket_speed_multiplier;
                rocket->rocket_sprite = ((tank.allignment == RED) ? &rocket_red : &rocket_blue);
                //cout << rockets_pool.to_string();
                //rockets.push_back(Rocket(tank.position, (target.get_position() - tank.position).normalized() * 3, rocket_radius, tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));

                tank.reload_rocket();
            }
        }
    }
}

/// <summary>
/// Update smoke plumes
/// </summary>
void Game::update_smoke() {
    for (Smoke& smoke :smokes){
        smoke.tick();
    }
}

/// <summary>
/// Calculates a convex hull around all the tanks
/// </summary>
void Game::find_concave_hull() {
    //Calculate "forcefield" around active tanks
    forcefield_hull.clear();

   //Find first active tank (this loop is a bit disgusting, fix?)
    int first_active = 0;
    for (Tank& tank : tanks)
    {
        if (tank.active)
        {
            break;
        }
        first_active++;
    }
    vec2 point_on_hull = tanks.at(first_active).position;
    //Find left most tank position

    for (Tank& tank : tanks)
    {
        if (tank.active)
        {
            if (tank.position.x <= point_on_hull.x)
            {
                point_on_hull = tank.position;
            }
        }
    }

    //Calculate convex hull for 'rocket barrier'
    while (true)
    {
        //Add last found point
        forcefield_hull.push_back(point_on_hull);

        //Loop through all points replacing the endpoint with the current iteration every time 
        //it lies left of the current segment formed by point_on_hull and the current endpoint.
        //By the end we have a segment with no points on the left and thus a point on the convex hull.
        vec2 endpoint = tanks.at(first_active).position;
        for (Tank& tank : tanks)
        {
            if (tank.active)
            {
                if ((endpoint == point_on_hull) || left_of_line(point_on_hull, endpoint, tank.position))
                {
                    endpoint = tank.position;
                }
            }
        }

        //Set the starting point of the next segment to the found endpoint.
        point_on_hull = endpoint;

        //If we went all the way around we are done.
        if (endpoint == forcefield_hull.at(0))
        {
            break;
        }
    }
}

/// <summary>
/// Loop over all the rockets, move and collide them with either the tanks or the concave hull
/// </summary>
void Game::update_rockets() {
    for (auto rocket_iterator = rockets_pool.used_items.begin(); rocket_iterator != rockets_pool.used_items.end(); )
    {
        Rocket* rocket = *rocket_iterator;
        bool found = false;
        rocket->tick();
        Tank* tank; 

        if (rocket->allignment == RED) {
            tank = tree.search_closest(root_Blue, rocket->position, 0)->tank;
        }
        else if (rocket->allignment == BLUE) {
            tank = tree.search_closest(root_Red, rocket->position, 0)->tank;
        }

        if (rocket->intersects(tank->position, tank->collision_radius))
        {
            explosions.push_back(Explosion(&explosion, tank->position));

            if (tank->hit(rocket_hit_value))
            {
                smokes.push_back(Smoke(smoke, tank->position - vec2(7, 24)));
            }

            rocket_iterator = rockets_pool.free(rocket_iterator);
            found = true;
            
        }

        if (!found) {
            ++rocket_iterator;
        }
        if (rocket_iterator > rockets_pool.used_items.end()) {
            break;
        }

    }

    //Disable rockets if they collide with the "forcefield"
    //Hint: A point to convex hull intersection test might be better here? :) (Disable if outside)
    for (auto rocket_iterator = rockets_pool.used_items.begin(); rocket_iterator != rockets_pool.used_items.end(); )
    {
        Rocket* rocket_ptr = *rocket_iterator;
        bool found = false;

        if (rocket_ptr->active)
        {
            for (size_t i = 0; i < forcefield_hull.size(); i++)
            {
                if (circle_segment_intersect(forcefield_hull.at(i), forcefield_hull.at((i + 1) % forcefield_hull.size()), rocket_ptr->position, rocket_ptr->collision_radius))
                {
                    explosions.push_back(Explosion(&explosion, rocket_ptr->position));
                    rocket_iterator = rockets_pool.free(rocket_iterator);
                    found = true;
                }
            }
        }
        if (!found) {
            ++rocket_iterator;
        }
        if (rocket_iterator > rockets_pool.used_items.end()) {
            break;
        }
    }

    //Remove exploded rockets with remove erase idiom
    //rockets.erase(std::remove_if(rockets.begin(), rockets.end(), [](const Rocket& rocket) { return !rocket.active; }), rockets.end());
}


/// <summary>
/// Update list particle beams
/// Add each task to the list
/// only let the main thread continue when all tasks are done.
/// </summary>
void Game::update_particle_beams() {

    vector<future<void>> futures_list;
    futures_list.reserve(particle_beams.size());

    for (int i = 0; i < particle_beams.size(); i++) {
        futures_list.emplace_back(threadpool.enqueue([=] {update_particle_beam(particle_beams, i, tanks, smokes); }));
    }

    for (future<void>& future : futures_list) {
        future.wait();
    }
}

/// <summary>
/// update one particle beam
/// </summary>
/// <param name="particle_beams">list of particle beams</param>
/// <param name="index">the index in the list of current particle beam </param>
/// <param name="tanks">list of tanks</param>
/// <param name="smokes">list of smokes</param>
void Tmpl8::Game::update_particle_beam(vector<Particle_beam>& particle_beams, int index, vector<Tank>& tanks, vector<Smoke>& smokes)
{
    Particle_beam& particle_beam = particle_beams.at(index);
    particle_beam.tick(tanks); //wHY USE TANKS?

    //Damage all tanks within the damage window of the beam (the window is an axis-aligned bounding box)
    for (Tank& tank : tanks)
    {
        if (tank.active && particle_beam.rectangle.intersects_circle(tank.get_position(), tank.get_collision_radius()))
        {
            if (tank.hit(particle_beam.damage))
            {
                smokes.push_back(Smoke(smoke, tank.position - vec2(0, 48)));
            }
        }
    }
}

/// <summary>
/// Update explosion tick function and remove when done with remove erase idiom
/// </summary>
void Game::update_explosions() {
    for (Explosion& explosion : explosions)
    {
        explosion.tick();
    }

    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& explosion) { return explosion.done(); }), explosions.end());
}


/// <summary>
/// Update the game state
/// </summary>
/// <param name="deltaTime"></param>
void Game::update(float deltaTime)
{
    //Initializing routes here so it gets counted for performance..
    if (frame_count == 0)
    {
        init_tank_routes();
    }


    nudge_and_collide_tanks();

    update_tanks();

    auto root_blue_future = threadpool.enqueue([=] {sort_nodes(&root_Blue, &to_be_sorted_childeren_blue); });
    auto root_red_future = threadpool.enqueue([=] {sort_nodes(&root_Red, &to_be_sorted_childeren_red); });
    auto particle_beams_future = threadpool.enqueue([=] {update_particle_beams(); });
    auto find_concave_hull_future = threadpool.enqueue([=] {find_concave_hull(); });

    find_concave_hull_future.wait();
    particle_beams_future.wait();
    root_blue_future.wait();
    root_red_future.wait();

    update_rockets();

    auto smoke_future = threadpool.enqueue([=] {update_smoke(); });
    auto explosions_future = threadpool.enqueue([=] {update_explosions(); });

    smoke_future.wait();
    explosions_future.wait();
}

/// <summary>
/// Draw all the sprites on screen
/// </summary>
void Game::draw()
{
    // clear the graphics window
    screen->clear(0);

    //Draw background
    background_terrain.draw(screen);

    //Draw sprites
    for (int i = 0; i < num_tanks_blue + num_tanks_red; i++)
    {
        tanks.at(i).draw(screen);

        vec2 tank_pos = tanks.at(i).get_position();
    }

    for (Rocket* rocket : rockets_pool.used_items)
    {
        rocket->draw(screen);
    }

    for (Smoke& smoke : smokes)
    {
        smoke.draw(screen);
    }

    for (Particle_beam& particle_beam : particle_beams)
    {
        particle_beam.draw(screen);
    }

    for (Explosion& explosion : explosions)
    {
        explosion.draw(screen);
    }

    //Draw forcefield (mostly for debugging, its kinda ugly..)
    for (size_t i = 0; i < forcefield_hull.size(); i++)
    {
        vec2 line_start = forcefield_hull.at(i);
        vec2 line_end = forcefield_hull.at((i + 1) % forcefield_hull.size());
        line_start.x += HEALTHBAR_OFFSET;
        line_end.x += HEALTHBAR_OFFSET;
        screen->line(line_start, line_end, 0x0000ff);
    }

    //Draw sorted health bars
    for (int t = 0; t < 2; t++)
    {
        const int NUM_TANKS = ((t < 1) ? num_tanks_blue : num_tanks_red);

        const int begin = ((t < 1) ? 0 : num_tanks_blue);
        //vector<Tank*> sorted_tanks;
        //sorted_tanks.reserve(tank_ptrs_sorted_on_health.size());
        Sort<Tank*, int(Tmpl8::Game::*)(Tank*), Game* >::binary_insertion_sort(tank_ptrs_sorted_on_health, &Game::get_health_from_ptr, this);
        //tank_ptrs_sorted_on_health = sorted_tanks;
        tank_ptrs_sorted_on_health.erase(std::remove_if(tank_ptrs_sorted_on_health.begin(), tank_ptrs_sorted_on_health.end(), [](const Tank* tank) { return !tank->active; }), tank_ptrs_sorted_on_health.end());


        draw_health_bars(tank_ptrs_sorted_on_health, t);
    }
}

/// <summary>
/// Sort tanks by health value using insertion sort
/// </summary>
/// <param name="original">the original list of tanks</param>
/// <param name="sorted_tanks">the sorted tanks</param>
/// <param name="begin"></param>
/// <param name="end"></param>
void Tmpl8::Game::insertion_sort_tanks_health(const std::vector<Tank>& original, std::vector<const Tank*>& sorted_tanks, int begin, int end)
{
    const int NUM_TANKS = end - begin;
    sorted_tanks.reserve(NUM_TANKS);
    sorted_tanks.emplace_back(&original.at(begin));

    for (int i = begin + 1; i < (begin + NUM_TANKS); i++)
    {
        const Tank& current_tank = original.at(i);

        for (int s = (int)sorted_tanks.size() - 1; s >= 0; s--)
        {
            const Tank* current_checking_tank = sorted_tanks.at(s);

            if ((current_checking_tank->compare_health(current_tank) <= 0))
            {
                sorted_tanks.insert(1 + sorted_tanks.begin() + s, &current_tank);
                break;
            }

            if (s == 0)
            {
                sorted_tanks.insert(sorted_tanks.begin(), &current_tank);
                break;
            }
        }
    }
}

int Tmpl8::Game::get_health_from_ptr(Tank* ptr)
{
    return ptr->health;
}
/// <summary>
/// draw the health bars
/// </summary>
/// <param name="sorted_tanks"></param>
/// <param name="team"></param>
void Tmpl8::Game::draw_health_bars(const std::vector<Tank*>& sorted_tanks, const int team)
{
    int health_bar_start_x = (team < 1) ? 0 : (SCRWIDTH - HEALTHBAR_OFFSET) - 1;
    int health_bar_end_x = (team < 1) ? health_bar_width : health_bar_start_x + health_bar_width - 1;

    for (int i = 0; i < SCRHEIGHT - 1; i++)
    {
        //Health bars are 1 pixel each
        int health_bar_start_y = i * 1;
        int health_bar_end_y = health_bar_start_y + 1;

        screen->bar(health_bar_start_x, health_bar_start_y, health_bar_end_x, health_bar_end_y, REDMASK);
    }

    //Draw the <SCRHEIGHT> least healthy tank health bars
    int draw_count = std::min(SCRHEIGHT, (int)sorted_tanks.size());
    for (int i = 0; i < draw_count - 1; i++)
    {
        //Health bars are 1 pixel each
        int health_bar_start_y = i * 1;
        int health_bar_end_y = health_bar_start_y + 1;

        float health_fraction = (1 - ((double)sorted_tanks.at(i)->health / (double)tank_max_health));

        if (team == 0) { screen->bar(health_bar_start_x + (int)((double)health_bar_width * health_fraction), health_bar_start_y, health_bar_end_x, health_bar_end_y, GREENMASK); }
        else { screen->bar(health_bar_start_x, health_bar_start_y, health_bar_end_x - (int)((double)health_bar_width * health_fraction), health_bar_end_y, GREENMASK); }
    }
}

/// <summary>
/// When we reach max_frames print the duration and speedup multiplier
/// Updating REF_PERFORMANCE at the top of this file with the value
/// on your machine gives you an idea of the speedup your optimizations give
/// </summary>
void Tmpl8::Game::measure_performance()
{
    char buffer[128];
    if (frame_count >= max_frames)
    {
        if (!lock_update)
        {
            duration = perf_timer.elapsed();
            cout << "Duration was: " << duration << " (Replace REF_PERFORMANCE with this value)" << endl;
            lock_update = true;
        }

        frame_count--;
    }

    if (lock_update)
    {
        screen->bar(420 + HEALTHBAR_OFFSET, 170, 870 + HEALTHBAR_OFFSET, 430, 0x030000);
        int ms = (int)duration % 1000, sec = ((int)duration / 1000) % 60, min = ((int)duration / 60000);
        sprintf(buffer, "%02i:%02i:%03i", min, sec, ms);
        frame_count_font->centre(screen, buffer, 200);
        sprintf(buffer, "SPEEDUP: %4.1f", REF_PERFORMANCE / duration);
        frame_count_font->centre(screen, buffer, 340);
    }
}

/// <summary>
/// the function that sorts the nodes
/// </summary>
/// <param name="root">the root</param>
/// <param name="tobesortedchilderen">the tobesortedchilderen</param>
void Tmpl8::Game::sort_nodes(KdTree::node** root, vector<KdTree::node*>* tobesortedchilderen)
{
    *root = tree.insert_nodes(*tree.get_tobe_sortedlist(*root, tobesortedchilderen, 0), 0);

    tobesortedchilderen->erase(tobesortedchilderen->begin(), tobesortedchilderen->end());
}

/// <summary>
/// Main application tick function
/// </summary>
void Game::tick(float deltaTime)
{
    if (!lock_update)
    {
        update(deltaTime);
    }
    draw();

    measure_performance();

    // print something in the graphics window
    //screen->Print("hello world", 2, 2, 0xffffff);

    // print something to the text window
    //cout << "This goes to the console window." << std::endl;

    //Print frame count
    frame_count++;
    string frame_count_string = "FRAME: " + std::to_string(frame_count);
    frame_count_font->print(screen, frame_count_string.c_str(), 350, 580);
}