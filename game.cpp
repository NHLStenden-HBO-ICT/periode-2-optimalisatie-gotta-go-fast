#include "precomp.h" // include (only) this in every .cpp file

constexpr auto num_tanks_blue = 2048;
constexpr auto num_tanks_red = 2048;

constexpr auto tank_max_health = 1000;
constexpr auto rocket_hit_value = 60;
constexpr auto particle_beam_hit_value = 50;

constexpr auto tank_max_speed = 1.0;

constexpr auto health_bar_width = 70;

constexpr auto max_frames = 2000;

//Global performance timer
constexpr auto REF_PERFORMANCE = 94653; //UPDATE THIS WITH YOUR REFERENCE PERFORMANCE (see console after 2k frames)
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

struct node* rootBlue;
struct node* rootRed;

vector<node*> tobesortedchilderen; //turn in vector

//===========================================
// Main bottlenecks
// ~ Single threading
// ~ nudge_and_collide_tanks (n^2)
// ~ update_tanks (n^2)
//===========================================

#pragma region Node/kdtree

//TODO COMMENT 
struct node
{
    Tank* tank; //pointer to a tank
    struct node* left;
    struct node* right;

};

struct node* newnode(Tank* tank) {

    struct node* node
        = (struct node*)malloc(sizeof(struct node));

    node->tank = tank;
    node->left = NULL;
    node->right = NULL;

    return node;
}

node* inserttank(node* root, Tank* tank, bool x, int i) {
    if (root == NULL) {
        //cout << i << std::endl;
        return newnode(tank);
    }

    i = i + 1;
    if (x==true) {

        if (tank->position.x <= root->tank->position.x) {
            root->left = inserttank(root->left, tank, !x, i);
            return root;
        }
        if (tank->position.x > root->tank->position.x) {
            root->right = inserttank(root->right, tank, !x, i);
            return root;
        }
    }

    if (x==false) {
        if (tank->position.y <= root->tank->position.y) {
           
            root->left = inserttank(root->left, tank, !x, i);
            return root;
        }
        if (tank->position.y > root->tank->position.y) {

            root->right = inserttank(root->right, tank, !x, i);
            return root;
        }
    }

}

node* insertnode(node* root, node* newnode, bool x, int i) {

    if (root == NULL) {
        return newnode;
    }

    //todo diepte interger for boom.
    //if x is true check on that level on the x-as otherwise check for y
    if (x==true) {
        if (newnode->tank->position.x <= root->tank->position.x) {
            root->left = insertnode(root->left, newnode, false, i++);
            return root;
        }
        if (newnode->tank->position.x > root->tank->position.x) {
            root->right = insertnode(root->right, newnode, false, i++);
            return root;
        }
    }

    if (x==false) {
        if (newnode->tank->position.y <= root->tank->position.y) {
            root->left = insertnode(root->left, newnode, true, i++);
            return root;
        }
        if (newnode->tank->position.y > root->tank->position.y) {
            root->right = insertnode(root->right, newnode, true, i++);
            return root;
        }
    }

}

node* searchClosest(node* root, Tank tank) {

    //todo interger diepte
    //check if left is shortest
    if (root->left != NULL && fabsf((root->left->tank->get_position() - tank.get_position()).sqr_length()) < fabsf((root->tank->get_position() - tank.get_position()).sqr_length())) {
        if (root->left->left == NULL && root->left->right == NULL) {
            return root->left;
        }
        return searchClosest(root->left, tank);
    }
    //check if right is shortest
    else if (root->right != NULL && fabsf((root->right->tank->get_position() - tank.get_position()).sqr_length()) < fabsf((root->tank->get_position() - tank.get_position()).sqr_length())) {
        if (root->right->left == NULL && root->right->right == NULL) {
            return root->right;
        }
        return searchClosest(root->right, tank);
    }
    else {
        return root;
    }

}

node* updateinsert(node* root, bool x, vector<node*>* tobesortedchilderen) {

    if (root->left != NULL && root->right != NULL) {
        
        if (root->left->left != NULL || root->left->right != NULL) {
            root->left = updateinsert(root->left, !x, tobesortedchilderen);
        }
        
        if (root->left->tank->active == true) {
            tobesortedchilderen->push_back(root->left);
            
        }

        if (root->right->right != NULL || root->right->left != NULL) {
            root->right = updateinsert(root->right, !x, tobesortedchilderen);
        }
        if (root->right->tank->active == true) {
            tobesortedchilderen->push_back(root->right);
            
        }
        root->left = NULL;
        root->right = NULL;
        
        return root;

    }

    if (root->right == NULL && root->left != NULL) {
        
        if (root->left->left != NULL || root->left->right != NULL) {
            root->left = updateinsert(root->left, !x, tobesortedchilderen);
        }

        if (root->left->tank->active == true) {
            tobesortedchilderen->push_back(root->left);
            
        }
        root->left = NULL;
        
        return root;
    }

    if (root->left == NULL && root->right != NULL) {
        
        if (root->right->right != NULL || root->right->left != NULL) {
            root->right = updateinsert(root->right, !x, tobesortedchilderen);
        }
        if (root->right->tank->active == true) {
            tobesortedchilderen->push_back(root->right);
            
        }
        root->right = NULL;
        
        return root;
    }

    if (root->left == NULL && root->right == NULL && root->tank->active == true) {
        cout << "root is active and needs to be added to list" << endl;
        tobesortedchilderen->push_back(root);
        node* newroot = tobesortedchilderen->at(tobesortedchilderen->size() - 1);

        return tobesortedchilderen->at(tobesortedchilderen->size());
    }

}

node* searchTankforRocketUpdate(node* root, Rocket& rocket, bool x, int i) {
    cout << i << endl;
    if (rocket.intersects(root->tank->position, root->tank->collision_radius))
    {//because you need the first one to intersect you can start by checking the top and then going down
        cout << "tank found at tree lvl : ";
        cout << i << endl;
        return root;
    }
    else if(x==true) {
        i = i + 1;
        if (rocket.position.x <= root->tank->position.x && root->left!=NULL)
        {
            return searchTankforRocketUpdate(root->left, rocket, !x, i);
        }
        else if (rocket.position.x > root->tank->position.x && root->right != NULL) {
            return searchTankforRocketUpdate(root->right, rocket, !x, i);
        }
    }
    else if (x == false) {
        i = i + 1;
        if (rocket.position.y <= root->tank->position.y && root->left != NULL)
        {
            return searchTankforRocketUpdate(root->left, rocket, !x, i);
        }
        else if (rocket.position.y > root->tank->position.y && root->right != NULL){
            return searchTankforRocketUpdate(root->right, rocket, !x, i);
        }
    }
    else {
        return NULL;
    }

}
#pragma endregion

// -----------------------------------------------------------
// Initialize the simulation state
// This function does not count for the performance multiplier
// (Feel free to optimize anyway though ;) )
// -----------------------------------------------------------
void Game::init()
{
    frame_count_font = new Font("assets/digital_small.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZ:?!=-0123456789.");

    tanks.reserve(num_tanks_blue + num_tanks_red);

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

    particle_beams.push_back(Particle_beam(vec2(590, 327), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));
    particle_beams.push_back(Particle_beam(vec2(64, 64), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));
    particle_beams.push_back(Particle_beam(vec2(1200, 600), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));

    //initialise tree first get root tank aka middlest tank 

    Tank* Rootblue = &tanks[(num_tanks_blue / 2)-1];
    Tank* Rootred = &tanks[(num_tanks_red / 2)+ num_tanks_blue - 1];

    rootBlue = inserttank(NULL,Rootblue,false,0);
    rootRed = inserttank(NULL,Rootred,false,0);

    for (int y = 0; y < tanks.size(); y++ ) {
        if (tanks[y].allignment == BLUE) {
           rootBlue = inserttank(rootBlue, &tanks[y], true, 0);

        }
        if (tanks[y].allignment == RED) { 
            rootRed = inserttank(rootRed, &tanks[y], true, 0);
        }
    }

}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::shutdown()
{
}

// -----------------------------------------------------------
// Iterates through all tanks and returns the closest enemy tank for the given tank
// O(n)=n
// n is the amount of tanks
// -----------------------------------------------------------
Tank& Game::find_closest_enemy(Tank& current_tank)
{
    //float closest_distance = numeric_limits<float>::infinity();
    //int closest_index = 0;

    if (current_tank.allignment == RED) {
        return *searchClosest(rootBlue, current_tank)->tank;
    }    
    if (current_tank.allignment == BLUE) {
        return *searchClosest(rootRed, current_tank)->tank;
    }
    /*
     
    for (int i = 0; i < tanks.size(); i++)
    {
        if (tanks.at(i).allignment != current_tank.allignment && tanks.at(i).active)
        {
            float sqr_dist = fabsf((tanks.at(i).get_position() - current_tank.get_position()).sqr_length());
            if (sqr_dist < closest_distance)
            {
                closest_distance = sqr_dist;
                closest_index = i;
            }
        }
    }

    return tanks.at(closest_index);
     
    */

}

// -----------------------------------------------------------
// Checks if a point lies on the left of an arbitrary angled line
// -----------------------------------------------------------
bool Tmpl8::Game::left_of_line(vec2 line_start, vec2 line_end, vec2 point)
{
    return ((line_end.x - line_start.x) * (point.y - line_start.y) - (line_end.y - line_start.y) * (point.x - line_start.x)) < 0;
}

// -----------------------------------------------------------
// Calculate the route to the destination for each tank using BFS
// O(n)=n*m 
// n is the amount of tanks
// m is the amount of terain exits that is used in get route
// -----------------------------------------------------------
void Game::init_tank_routes() {
    for (Tank& t : tanks)
    {
        t.set_route(background_terrain.get_route(t, t.target));
    }
}

// -----------------------------------------------------------
// Check tank collision and nudge tanks away from each other
// O(n^2)=n^2
// n is the amount of tanks
// -----------------------------------------------------------
void Game::nudge_and_collide_tanks() {
    for (Tank& tank : tanks)
    {
        if (tank.active)
        {
            for (Tank& other_tank : tanks)
            {
                if (&tank == &other_tank || !other_tank.active) continue;

                vec2 dir = tank.get_position() - other_tank.get_position();
                float dir_squared_len = dir.sqr_length();

                float col_squared_len = (tank.get_collision_radius() + other_tank.get_collision_radius());
                col_squared_len *= col_squared_len;

                if (dir_squared_len < col_squared_len)
                {
                    tank.push(dir.normalized(), 1.f);
                }
            }
        }
    }
}

// -----------------------------------------------------------
// Update tanks 
// O(n^2)=n^2
// n is the amount of tanks
// -----------------------------------------------------------
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

                rockets.push_back(Rocket(tank.position, (target.get_position() - tank.position).normalized() * 3, rocket_radius, tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));

                tank.reload_rocket();
            }
        }
    }
}

// -----------------------------------------------------------
// Update smoke plumes
// O(n)=n
// n is the amount of smoke
// -----------------------------------------------------------
void Game::update_smoke() {
    for (Smoke& smoke : smokes)
    {
        smoke.tick();
    }
}

// -----------------------------------------------------------
// Calculates a convex hull around all the tanks
// O(n)=2*n
// n is the amount of tanks
// -----------------------------------------------------------
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

// -----------------------------------------------------------
// Loop over all the rockets, move and collide them with either the tanks or the concave hull
// O(n)=n*m + n*k = n(m+k)
// n is the amount of rockets
// m is the amount of tanks
// k is the size of the forcefield hull
// -----------------------------------------------------------
void Game::update_rockets() {
    for (Rocket& rocket : rockets)
    {
        rocket.tick();
        
        node* tree =NULL;

        if (rocket.allignment == RED) {
            tree = rootBlue;
        }
        else if (rocket.allignment == BLUE)
        {
            tree = rootRed;
        }
        
        if (searchTankforRocketUpdate(tree, rocket, true,0) != NULL)
        {
            Tank* tank = searchTankforRocketUpdate(tree, rocket, true,0)->tank;

            explosions.push_back(Explosion(&explosion, tank->position));

            if (tank->hit(rocket_hit_value))
            {
                smokes.push_back(Smoke(smoke, tank->position - vec2(7, 24)));
            }

            rocket.active = false;
            break;
        }
        //Check if rocket collides with enemy tank, spawn explosion, and if tank is destroyed spawn a smoke plume
        /*for (Tank& tank : tanks)
        {
            if (tank.active && (tank.allignment != rocket.allignment) && rocket.intersects(tank.position, tank.collision_radius))
            {
                explosions.push_back(Explosion(&explosion, tank.position));

                if (tank.hit(rocket_hit_value))
                {
                    smokes.push_back(Smoke(smoke, tank.position - vec2(7, 24)));
                }

                rocket.active = false;
                break;
            }
        }*/
        
    }

    //Disable rockets if they collide with the "forcefield"
    //Hint: A point to convex hull intersection test might be better here? :) (Disable if outside)
    for (Rocket& rocket : rockets)
    {
        if (rocket.active)
        {
            for (size_t i = 0; i < forcefield_hull.size(); i++)
            {
                if (circle_segment_intersect(forcefield_hull.at(i), forcefield_hull.at((i + 1) % forcefield_hull.size()), rocket.position, rocket.collision_radius))
                {
                    explosions.push_back(Explosion(&explosion, rocket.position));
                    rocket.active = false;
                }
            }
        }
    }



    //Remove exploded rockets with remove erase idiom
    rockets.erase(std::remove_if(rockets.begin(), rockets.end(), [](const Rocket& rocket) { return !rocket.active; }), rockets.end());
}

// -----------------------------------------------------------
//Update particle beams
// O(n)=n*m
// n is the amount of tanks
// m is the amount of Particle beams
// -----------------------------------------------------------
void Game::update_particle_beams() {
    for (Particle_beam& particle_beam : particle_beams)
    {
        particle_beam.tick(tanks);

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
}

// -----------------------------------------------------------
//Update explosion sprites and remove when done with remove erase idiom
// O(n)=n
// n is the amount of explosions
// -----------------------------------------------------------
void Game::update_explosions() {
    for (Explosion& explosion : explosions)
    {
        explosion.tick();
    }

    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& explosion) { return explosion.done(); }), explosions.end());
}


// -----------------------------------------------------------
// Update the game state:
// Move all objects
// Update sprite frames
// Collision detection
// Targeting etc..
// -----------------------------------------------------------
void Game::update(float deltaTime)
{   
    //Initializing routes here so it gets counted for performance..
    if (frame_count == 0)
    {
        init_tank_routes();
    }
    
    
    nudge_and_collide_tanks();

    
    update_tanks();
    
    update_smoke();
    
    find_concave_hull();
    
    update_rockets();
    
    update_particle_beams();
    
    update_explosions();    

    
    rootBlue = updateinsert(rootBlue, true, &tobesortedchilderen);
   
    for (node* nd : tobesortedchilderen) {
        rootBlue = insertnode(rootBlue, nd, true, 0);
    }
    tobesortedchilderen.erase(tobesortedchilderen.begin(), tobesortedchilderen.end());
    
    rootRed = updateinsert(rootRed, true, &tobesortedchilderen);
    for (node* nd : tobesortedchilderen) {
        rootRed = insertnode(rootRed, nd, true, 0);
    }
    tobesortedchilderen.erase(tobesortedchilderen.begin(), tobesortedchilderen.end());
}

// -----------------------------------------------------------
// Draw all sprites to the screen
// (It is not recommended to multi-thread this function)
// -----------------------------------------------------------
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

    for (Rocket& rocket : rockets)
    {
        rocket.draw(screen);
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
        std::vector<const Tank*> sorted_tanks;
        insertion_sort_tanks_health(tanks, sorted_tanks, begin, begin + NUM_TANKS);
        sorted_tanks.erase(std::remove_if(sorted_tanks.begin(), sorted_tanks.end(), [](const Tank* tank) { return !tank->active; }), sorted_tanks.end());

        draw_health_bars(sorted_tanks, t);
    }
}

// -----------------------------------------------------------
// Sort tanks by health value using insertion sort
// -----------------------------------------------------------
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

// -----------------------------------------------------------
// Draw the health bars based on the given tanks health values
// -----------------------------------------------------------
void Tmpl8::Game::draw_health_bars(const std::vector<const Tank*>& sorted_tanks, const int team)
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

// -----------------------------------------------------------
// When we reach max_frames print the duration and speedup multiplier
// Updating REF_PERFORMANCE at the top of this file with the value
// on your machine gives you an idea of the speedup your optimizations give
// -----------------------------------------------------------
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

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
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
