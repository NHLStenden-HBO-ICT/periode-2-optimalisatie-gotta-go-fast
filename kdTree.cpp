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

        if (endL >= 0) { //if list left has potential kids
            vector<node*> ListL = vector<node*>(list.begin(), list.begin() + endL);
            median->left = insertnodes(ListL, i);
        }

        if (beginR <= list.size()) {//if list right has potential kids
            vector<node*> ListR = vector<node*>(list.begin() + beginR, list.end());
            median->right = insertnodes(ListR, i);
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
    kdTree::node* kdTree::searchClosest(node* root, vec2 point, int i) {

        if (root == NULL) {
            return NULL;
        }

        node* closestbranch;
        node* otherbranch;

        //if i aka the depth is even that means the check happens on x axis, otherwise it happens on the y axis
        if (( point.x< root->tank->position.x && i%2==0)||(point.y < root->tank->position.y && i % 2 != 0)) { //if target is left

            closestbranch = root->left;
            otherbranch = root->right;

        }
        else //if target is in the right branch
        {
            closestbranch = root->right;
            otherbranch = root->left;
        }

        node* temp = searchClosest(closestbranch, point, i + 1);

        node* closest = getclosest(point, temp, root);

        float distanceclosest = getdistancesquared(closest->tank->position, point, 0) + getdistancesquared(closest->tank->position, point, 1); // calculation of r

        if (getdistancesquared(root->tank->position,point, i)<= distanceclosest) {
            //calculation of r' and see if its smaller.
            //depending on the depth check if the distance to the other branch is not smaller.

            node* temp = searchClosest(otherbranch, point, i + 1);

            closest = getclosest(point, temp, closest);
        }

        return closest;
    }
    
    /// <summary>
    /// this function is almost a direct copy of searchclosest but because the check if the tank and the tank in the node is the same it has to be different.
    /// </summary>
    /// <param name="root">root of the tree</param>
    /// <param name="tank"> the tank of which the closest is being checked</param>
    /// <param name="i">depth</param>
    /// <returns>the node with closest tank in this tree</returns>
    kdTree::node* kdTree::searchClosestOtherTank(node* root, Tank* tank, int i)
    {
        if (root == NULL) {
            return NULL;
        }

        node* closestbranch;
        node* otherbranch;

        //if i aka the depth is even that means the check happens on x axis, otherwise it happens on the y axis
        if ((tank->position.x < root->tank->position.x && i % 2 == 0) || (tank->position.y < root->tank->position.y && i % 2 != 0)) { //if target is left

            closestbranch = root->left;
            otherbranch = root->right;
        }
        else //if target is in the right branch
        {
            closestbranch = root->right;
            otherbranch = root->left;
        }

        node* temp = searchClosestOtherTank(closestbranch, tank,i + 1);

        node* closest = getclosest(tank, temp, root);

        float distanceclosest = getdistancesquared(closest->tank->position, tank->position, 0) + getdistancesquared(closest->tank->position, tank->position, 1); // calculation of r

        if (getdistancesquared(root->tank->position, tank->position, i) <= distanceclosest) {
            //calculation of r' and see if its smaller.
            //depending on the depth check if the distance to the other branch is not smaller.

            node* temp = searchClosestOtherTank(otherbranch, tank, i + 1);

            closest = getclosest(tank, temp, closest);
        }

        return closest;
    }

    /// <summary>
    /// compare 2 nodes to see which is the closest to target
    /// </summary>
    /// <param name="target"> the target point </param>
    /// <param name="node1"> the first node which is checked </param>
    /// <param name="node2"> the second node thats being checked </param>
    /// <returns> the node thats closest to the target.</returns>
    kdTree::node* kdTree::getclosest(vec2 target, node* node1, node* node2)
    {
        if (node1 == NULL){
            return node2;
        }      

        if (node2 == NULL){
            return node1;
        }


        if (((node1->tank->position.x - target.x)* (node1->tank->position.x - target.x)) + ((node1->tank->position.y - target.y)* (node1->tank->position.y - target.y)) < ((node2->tank->position.x - target.x) * (node2->tank->position.x - target.x)) + ((node2->tank->position.y - target.y) * (node2->tank->position.y - target.y))) {

            return node1;
        }
        else {
            return node2;
        }
    }


    /// <summary>
    /// compare 2 nodes to see which is the closest to target
    /// it also checks wether the tank in the nodes is the same as the target.
    /// if that is the case give the other node as the closest.
    /// </summary>
    /// <param name="target"the tank></param>
    /// <param name="node1">the first node which is checked </param>
    /// <param name="node2"> the second node thats being checked </param>
    /// <returns> the node thats closest to the target.</returns>
    kdTree::node* kdTree::getclosest(Tank* target, node* node1, node* node2)
    {
        if (node1 == NULL) {
            return node2;
        }

        if (node2 == NULL) {
            return node1;
        }

        if (target == node1->tank) {
            return node2;
        }

        if (target == node2->tank) {
            return node1;
        }


        if (getdistancesquared(node1->tank->position, target->position,0) + getdistancesquared(node1->tank->position, target->position,1) < getdistancesquared(node2->tank->position, target->position, 0) + getdistancesquared(node2->tank->position, target->position, 1)) {

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

                if (y % 2 == 0) {
                    if (currentnode->tank->position.x < checkingnode->tank->position.x) {

                        list[s] = currentnode;
                        list[s + 1] = checkingnode;
                    }
                    else if (currentnode->tank->position.x == checkingnode->tank->position.x && currentnode->tank->position.y < checkingnode->tank->position.y) {

                        list[s] = currentnode;
                        list[s + 1] = checkingnode;
                    }
                    else
                    {
                        break;
                    }
                }
                else if (y % 2 != 0) {
                    if (currentnode->tank->position.y < checkingnode->tank->position.y && y % 2 != 0) {
                        list[s] = currentnode;
                        list[s + 1] = checkingnode;
                    }
                    else if (currentnode->tank->position.y == checkingnode->tank->position.y && currentnode->tank->position.x < checkingnode->tank->position.x && y % 2 != 0) {
                        list[s] = currentnode;
                        list[s + 1] = checkingnode;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        return list;
    }

    /// <summary>
    /// gets the square of the vec2 based of int i or the depth of the tree 
    /// </summary>
    /// <param name="t1">first vector </param>
    /// <param name="t2">second vector </param>
    /// <param name="i"> depth of the tree, determines wether x or y axis</param>
    /// <returns> the squared floatnumber </returns>
    float kdTree::getdistancesquared(vec2 t1, vec2 t2, int i )
    {
        if (i % 2 == 0) {
            return (t1.x - t2.x) * (t1.x - t2.x);
        }
        else
        {
            return (t1.y - t2.y) * (t1.y - t2.y);
        }

    }

}