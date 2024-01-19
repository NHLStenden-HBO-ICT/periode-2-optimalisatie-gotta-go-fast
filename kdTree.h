#pragma once

namespace Tmpl8 {
    class Tank;
    class Rocket;

     class KdTree
     {

     public:
        KdTree();
        struct node
        {
            Tank* tank;
            struct node* left;
            struct node* right;

        };

        node* newnode(Tank* tank);

        node* insert_nodes(vector<node*> list, int i);

        node* search_closest(node* root, vec2 point, int i);

        node* search_closest_other_tank(node* root, Tank* tank, int i);

        node* get_closest(vec2 target, node* node1, node* node2);        
        
        node* get_closest(Tank* target, node* node1, node* node2);

        vector<node*>* get_tobe_sortedlist(node* root, vector<node*>* tobesortedchilderen, int i);

        float get_tank_x(node* node_ptr);
        float get_tank_y(node* node_ptr);

        void sort_nodes(vector<node*>& input, int i);

        float get_distance_squared(vec2 t1, vec2 t2,int i);
    };
}


