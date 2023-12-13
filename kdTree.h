#pragma once

namespace Tmpl8 {
    class Tank;
    class Rocket;

     class kdTree
     {

     public:
        kdTree();
        // -----------------------------------------------------------
        // Initialize the node
        // contains a pointer to tank, pointer to its right and left child
        // -----------------------------------------------------------
        struct node
        {
            Tank* tank;
            struct node* left;
            struct node* right;

        };

        node* newnode(Tank* tank);

        node* insertnodes( vector<node*> list, int i);

        node* searchClosest(node* root, Tank tank, int i);

        node* getclosest(vec2 target, node* node1, node* node2);

        vector<node*>* get_tobe_sortedlist(node* root, vector<node*>* tobesortedchilderen, int i);

        vector<node*> sortnodes(vector<node*> list, int i);


    };
}


