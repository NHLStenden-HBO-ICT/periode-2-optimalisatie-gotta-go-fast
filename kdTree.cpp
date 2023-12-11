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
    /// creates a new tree based on the list of tanks.
    /// </summary>
    /// <param name="list">the list of tanks to be put into the tree</param>
    /// <param name="i"> depth of the tree</param>
    /// <returns>the root node of the tree</returns>
    kdTree::node* kdTree::inserttanks(vector<Tank*> list, int i)
    {
        if (list.size() == 0) {
            return NULL;
        }

        int medianindex = (list.size() - 1) / 2;

        Tank* tank = list[medianindex];

        node* median = newnode(tank);


        int endL = medianindex;
        int beginR = medianindex + 1;
        i = i + 1;

        if (endL < 0) { //if list left has no more potential kids
            return NULL;
        }

        if (beginR > list.size()) {//if list right has no more potential kids
            return NULL;
        }

        if (endL >= 0) { //if list left has no more potential kids
            vector<Tank*> ListL = vector<Tank*>(list.begin(), list.begin() + endL);
            median->left = inserttanks(ListL, i);
        }
        if (beginR <= list.size()) {//if list right has no more potential kids
            vector<Tank*> ListR = vector<Tank*>(list.begin() + beginR, list.end());
            median->right = inserttanks(ListR, i);
        }
        return median;
    }

    /// <summary>
    /// it searches for the closest node to tank, using an itterative style.
    /// it does so by first checking if root is not null.
    /// it then checks if its at a even or uneven depth.
    /// even depths means its getting checked on the x axis
    /// on uneven depths its getting checked on y axis.
    /// </summary>
    /// <param name="root">root node of the tree</param>
    /// <param name="tank">the tank of which its position is used to see which node is closest</param>
    /// <param name="i">the currrent depth of the tree</param>
    /// <returns>the node that is the closest</returns>
    kdTree::node* kdTree::searchClosest(node* root, Tank tank, int i) {

        float roottopoint;
        float lefttopoint;
        float righttopoint;

        if (root == NULL) {
            return NULL;
        }

        //if i aka the depth is even that means the check happens on x axis, otherwise it happens on the y axis
        if (i%2==0) {
            roottopoint = pow(2.0f, (root->tank->get_position().x - tank.get_position().x));
            if (root->left != NULL) {
                lefttopoint = pow(2.0f, (root->left->tank->get_position().x - tank.get_position().x));
            }
            if (root->right != NULL) {
                righttopoint = pow(2.0f, (root->right->tank->get_position().x - tank.get_position().x));
            }
        }
        else
        {
            roottopoint = pow(2.0f, root->tank->get_position().y - tank.get_position().y);
            if (root->left != NULL) {
                lefttopoint = pow(2.0f, (root->left->tank->get_position().y - tank.get_position().y));
            }
            if (root->right != NULL) {
                righttopoint = pow(2.0f, (root->right->tank->get_position().y - tank.get_position().y));
            }
        }
        
        i = i + 1; //add 1`to depth
        if (root->left != NULL && lefttopoint < roottopoint) {
            if (root->right != NULL && righttopoint < lefttopoint){
                
                return searchClosest(root->right, tank,i);
            }
            return searchClosest(root->left, tank,i);
        }
        //check if right is shortest as long as left is not shorter than root
        else if (root->right != NULL && righttopoint < roottopoint) {
            return searchClosest(root->right, tank,i);
        }
        else {
            return root;
        }

    }
    
    /// <summary>
    /// this searches the closest node tank to a certainpoint.
    /// works itterative
    /// </summary>
    /// <param name="root"> a pointer to a root</param>
    /// <param name="point"> the point which is being compared to </param>
    /// <returns>the node with the closest tank</returns>
    kdTree::node* kdTree::searchClosest(node* root, vec2 point) {
            if (root->left != NULL && fabsf((root->left->tank->get_position() - point).sqr_length()) < fabsf((root->tank->get_position() - point).sqr_length())) {
            if (root->right != NULL && fabsf((root->right->tank->get_position() - point).sqr_length()) < fabsf((root->left->tank->get_position() - point).sqr_length())){
                return searchClosest(root->right, point);
            }
            return searchClosest(root->left, point);
        }
        //check if right is shortest as long as left is not shorter than root
        else if (root->right != NULL && fabsf((root->right->tank->get_position() - point).sqr_length()) < fabsf((root->tank->get_position() - point).sqr_length())) {
            return searchClosest(root->right, point);
        }
        else {
            return root;
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
    vector<kdTree::node*> kdTree::sortnodes(vector<node*> list, int i)
    {
        for ( int i = 0; i < list.size(); i++)
        {
            node* currentnode = list[i];

            for (int s = i - 1; s >= 0; s--) {

                node* checkingnode = list[s];

                if (currentnode->tank->position.x < checkingnode->tank->position.x && i%2==0) {

                    list[s] = currentnode;
                    list[s + 1] = checkingnode;
                }
                else if(currentnode->tank->position.y < checkingnode->tank->position.y && i % 2 != 0){
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