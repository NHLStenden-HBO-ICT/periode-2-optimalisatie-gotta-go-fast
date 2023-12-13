#include "precomp.h"
#include "kdTree.h"

namespace Tmpl8 {
	kdTree::kdTree()
	{

	}

    /// <summary>
    /// creates a new node
    /// </summary>
    /// <param name="tank"> a pointer to a tank </param>
    /// <returns>returns a pointer to the just made node</returns>
    kdTree::node* kdTree::newnode(Tank* tank)
	{

        struct node* node
            = (struct node*)malloc(sizeof(struct node));

        node->tank = tank;
        node->left = NULL;
        node->right = NULL;

        return node;
        
	}
    

    /// <summary>
    /// creates a new tree based of the list of existing nodes
    /// its sorts the list and then finds the median of the list and then split them in sublists and give it to the childnodes
    /// </summary>
    /// <param name="list">the list of existing nodes</param>
    /// <param name="i">the depth of the tree</param>
    /// <returns>the new root node of the tree</returns>
    kdTree::node* kdTree::insertnodes(vector<node*> list, int i)
    {
        if (list.size() == 0) {
            return NULL;
        }

        list = sortnodes(list, i);

        int medianindex = (list.size()-1)/2;

        node* median = list[medianindex];


        int endL = medianindex;
        int beginR = medianindex+1;

        if (endL < 0) { //if list left has no more potential kids
            return NULL;
        }

        if (beginR > list.size()) {//if list right has no more potential kids
            return NULL;
        }
        
        i = i + 1;

        if (beginR <= list.size()) {//if list right has potential kids
            vector<node*> ListR = vector<node*>(list.begin() + beginR, list.end());
            median->right = insertnodes(ListR, i);
        }

        if (endL >= 0) { //if list left has potential kids
            vector<node*> ListL = vector<node*>(list.begin(), list.begin() + endL);
            median->left = insertnodes(ListL, i);
        }

        return median;
    }

    /// <summary>
    /// it searches for the closest node to tank, using an itterative style.
    /// it does so by first checking if root is not null.
    /// it then checks if its at a even or uneven depth.
    /// even depths means its getting checked on the x axis
    /// on uneven depths its getting checked on y axis.
    /// theorie from : https://www.youtube.com/watch?v=Glp7THUpGow
    /// </summary>
    /// <param name="root">root node of the tree</param>
    /// <param name="tank">the tank of which its position is used to see which node is closest</param>
    /// <param name="i">the currrent depth of the tree</param>
    /// <returns>the node that is the closest</returns>
    kdTree::node* kdTree::searchClosest(node* root, Tank tank, int i) {

        if (root == NULL) {
            return NULL;
        }

        node* closest = root;

        float rootdistance = pow(2, (root->tank->position.x - tank.position.x)) + pow(2, (root->tank->position.y - tank.position.y));

        //if i aka the depth is even that means the check happens on x axis, otherwise it happens on the y axis
        if (( tank.position.x< root->tank->position.x && i%2==0)||(tank.position.y < root->tank->position.y && i % 2 != 0)) { //if target x is below root x 

            node* temp = searchClosest(root->left, tank, i + 1);
            
            closest = getclosest(tank.position, temp, closest);

            float distanceclosest = pow(2, (closest->tank->position.x - tank.position.x)) + pow(2, (closest->tank->position.y - tank.position.y));

            if ((i % 2 == 0 && pow(2, root->tank->position.x - tank.position.x) < distanceclosest)||(i % 2 != 0 && pow(2, root->tank->position.y - tank.position.y) < distanceclosest)) {

                node* temp = searchClosest(root->right, tank, i + 1);

                closest = getclosest(tank.position, temp, closest);
            }

        }
        else //if target x is above root x 
        {

            node* temp = searchClosest(root->right, tank, i + 1);

            closest = getclosest(tank.position, temp, closest);

            float distanceclosest = pow(2, (closest->tank->position.x - tank.position.x)) + pow(2, (closest->tank->position.y - tank.position.y));

            if ((i % 2 == 0 && pow(2, root->tank->position.x - tank.position.x) < distanceclosest) || (i % 2 != 0 && pow(2, root->tank->position.y - tank.position.y) < distanceclosest)) {

                node* temp = searchClosest(root->left, tank, i + 1);

                closest = getclosest(tank.position, temp, closest);
            }
        }

        return closest;
    }

    kdTree::node* kdTree::getclosest(vec2 target, node* node1, node* node2)
    {
        if (node1 == NULL){
            return node2;
        }      

        if (node2 == NULL){
            return node1;
        }


        if (pow(2, (node1->tank->position.x - target.x)) + pow(2, (node1->tank->position.y - target.y)) < pow(2, (node2->tank->position.x - target.x)) + pow(2, (node2->tank->position.y - target.y))) {

            return node1;
        }
        else {
            return node2;
        }
    }

    /// <summary>
    /// gets every node from the tree whos tank is active
    /// </summary>
    /// <param name="root">the root node </param>
    /// <param name="tobesortedchilderen">the list that will be filled with kids</param>
    /// <param name="i">the depths of the tree</param>
    /// <returns>returns the filled list</returns>
    vector<kdTree::node*>* kdTree::get_tobe_sortedlist(node* root, vector<node*>* tobesortedchilderen, int i)
    {
        if (root == NULL) {
            return tobesortedchilderen;
        }
        
        if (root->left != NULL) {
            tobesortedchilderen = get_tobe_sortedlist(root->left, tobesortedchilderen, i + 1);
        }


        if (root->right != NULL) {
            tobesortedchilderen = get_tobe_sortedlist(root->right, tobesortedchilderen, i + 1);
        }

        
        if (root->tank->active == true)
        {

          tobesortedchilderen->push_back(root);

        }

        return tobesortedchilderen;
    }

    /// <summary>
    /// sort the nodes based on x or y 
    /// </summary>
    /// <param name="list"> the list of nodes to be sorted</param>
    /// <param name="i"> the depth which determines whether x or y even x uneven y</param>
    /// <returns>the sorted list</returns>
    vector<kdTree::node*> kdTree::sortnodes(vector<node*> list, int y)
    {
        for ( int i = 0; i < list.size(); i++)
        {
            node* currentnode = list[i];

            for (int s = i - 1; s >= 0; s--) {

                node* checkingnode = list[s];

                if (currentnode->tank->position.x < checkingnode->tank->position.x && y%2==0) {

                    list[s] = currentnode;
                    list[s + 1] = checkingnode;
                }
                else if(currentnode->tank->position.y < checkingnode->tank->position.y && y % 2 != 0){
                    list[s] = currentnode;
                    list[s + 1] = checkingnode;
                }
                else
                {
                    break;
                }
            }
        }

        return list;
    }

    /* Backup
    *    kdTree::node* kdTree::insertnode(vector<node*> nodelist,int parentindex, int rootindex, bool left, int i)
    {
        int medianindex;
        
        if (parentindex > rootindex) {

            if (left == false) {
                medianindex = ((parentindex - rootindex) / 2) + parentindex;
            }
            else
            {
                medianindex = parentindex - ((parentindex - rootindex) / 2);
            }
        }
        else
        {
            medianindex = (parentindex / 2);
        }
        cout << medianindex;
        node* median = nodelist[medianindex];

        if (nodelist.size() == 2) {
            median->left = insertnode(nodelist, medianindex, rootindex,true,i++);
            
        }
        else if (nodelist.size() > 2)
        {
            median->left = insertnode(nodelist, medianindex, rootindex, true, i++);

            median->right = insertnode(nodelist, medianindex,rootindex, false, i++);
        }

        return median;
    }
    /// <summary>
    /// currently checks for each node if they intersect.
    /// </summary>
    /// <param name="root"></param>
    /// <param name="rocket"></param>
    /// <returns></returns>
    kdTree::node* kdTree::rockethitsearch(node* root, Rocket* rocket) {

        if (rocket->intersects(root->tank->position, root->tank->collision_radius)) {//check if root intersects with rocket
            
            return root;
        }
        else //if root does not check the childeren
        {
            float roottorocketpos = fabsf((root->tank->get_position() - rocket->position).sqr_length());

            if (root->left != NULL){ //start with checking if left exists

                float lefttorocketpos = fabsf((root->left->tank->get_position() - rocket->position).sqr_length());
                if (root->right != NULL) {//check if right exists

                    float righttorocketpos = fabsf((root->right->tank->get_position() - rocket->position).sqr_length());
                    if (lefttorocketpos< roottorocketpos && righttorocketpos > roottorocketpos ){ //check if the lenght between left and rocket is smaller than root and rocket but greater than right and rocket.

                    }


                }
                else //if right doesn't exist
                {
                    node* node = rockethitsearch(root->left, rocket);
                    if (node != NULL)
                    {
                        return node;
                    }
                }
            }

            if (root->right != NULL) {
                node* node = rockethitsearch(root->right, rocket);

                if (node!= NULL)
                {
                    return node;
                }
                else
                {
                    if (root->left != NULL) {
                        return rockethitsearch(root->left, rocket);
                    }
                } 
            }
            return NULL; //no intersecting tank

        }

    }
    */


}