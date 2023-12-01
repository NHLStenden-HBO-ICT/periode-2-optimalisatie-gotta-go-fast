#pragma once

namespace Tmpl8 {
    class Tank;


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

        node* inserttank(node* root, Tank* tank, bool x, int i);

        node* insertnode(node* root, node* newnode, bool x, int i);

        node* searchClosest(node* root, Tank tank);

        node* searchClosest(node* root, vec2 point);

        node* getmostlefttank(node* root, bool x);

        node* updateinsert(node* root, bool x, vector<node*>* tobesortedchilderen);

    };
}


