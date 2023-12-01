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
    /// inserts tanks into the tree based on x/or y axis.
    /// it work itterative
    /// </summary>
    /// <param name="root">a pointeer to the root node of the tree/branch</param>
    /// <param name="tank">pointer to the tank that will be inserted</param>
    /// <param name="x"> a bool that determines whether the splitting happens on the x or y axis.</param>
    /// <param name="i"> the depth of the tree</param>
    /// <returns> returns the root node of the tree</returns>
    kdTree::node* kdTree::inserttank(node* root, Tank* tank, bool x, int i) {
        if (root == NULL) {
            //cout << i << std::endl;
            return newnode(tank);
        }

        i = i + 1;
        if (x == true) {

            if (tank->position.x <= root->tank->position.x) {
                root->left = inserttank(root->left, tank, !x, i);
                return root;
            }
            if (tank->position.x > root->tank->position.x) {
                root->right = inserttank(root->right, tank, !x, i);
                return root;
            }
        }

        if (x == false) {
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

    /// <summary>
    /// inserts a existing node into the tree based on x/or y axis.
    /// it work itterative
    /// </summary>
    /// <param name="root">a pointeer to the root node of the tree/branch</param>
    /// <param name="newnode">pointer to the node that will be inserted</param>
    /// <param name="x"> a bool that determines whether the splitting happens on the x or y axis.</param>
    /// <param name="i"> the depth of the tree</param>
    /// <returns> returns the root node of the tree</returns>
    kdTree::node* kdTree::insertnode(node* root, node* newnode, bool x, int i) {

        if (root == NULL) {
            return newnode;
        }

        //todo diepte interger for boom.
        //if x is true check on that level on the x-as otherwise check for y
        if (x == true) {
            if (newnode->tank->position.x <= root->tank->position.x) {
                root->left = insertnode(root->left, newnode, false, i++);
                return root;
            }
            if (newnode->tank->position.x > root->tank->position.x) {
                root->right = insertnode(root->right, newnode, false, i++);
                return root;
            }
        }

        if (x == false) {
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

    /// <summary>
    /// this searches the closest node tank to a tank.
    /// works itterative
    /// TODO change sqr to just a^2+b^2
    /// </summary>
    /// <param name="root"> a pointer to a root</param>
    /// <param name="tank"> the tank which is being compared to </param>
    /// <returns>the node with the closest tank</returns>
    kdTree::node* kdTree::searchClosest(node* root, Tank tank) {

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

    /// <summary>
    /// this searches the closest node tank to a certainpoint.
    /// works itterative
    /// TODO change sqr to just a^2+b^2
    /// </summary>
    /// <param name="root"> a pointer to a root</param>
    /// <param name="point"> the point which is being compared to </param>
    /// <returns>the node with the closest tank</returns>
    kdTree::node* kdTree::searchClosest(node* root, vec2 point) {

        //check if left is shortest
        if (root->left != NULL && fabsf((root->left->tank->get_position() - point).sqr_length()) < fabsf((root->tank->get_position() - point).sqr_length())) {
            if (root->left->left == NULL && root->left->right == NULL) {
                return root->left;
            }
            return searchClosest(root->left, point);
        }
        //check if right is shortest
        else if (root->right != NULL && fabsf((root->right->tank->get_position() - point).sqr_length()) < fabsf((root->tank->get_position() - point).sqr_length())) {
            if (root->right->left == NULL && root->right->right == NULL) {
                return root->right;
            }
            return searchClosest(root->right, point);
        }
        else {
            return root;
        }

    }

    /// <summary>
    /// get the most left tank that exist by checking all left
    /// usefull for the hull check
    /// </summary>
    /// <param name="root">pointer to the root of the tree</param>
    /// <param name="x">bool of the axis</param>
    /// <returns>the node thats most left</returns>
    kdTree::node* kdTree::getmostlefttank(node* root, bool x)
    {
        if (x==true) //split on y ax
        {
            if (root->left == NULL){
                return root;
            }
            return getmostlefttank(root->left,!x);
        }
        else if(x == false){ //split on x ax
            if (root->right ==NULL&& root->left==NULL)
            {
                return root;
            }
            else if (root->right == NULL && root->left != NULL)
            {
                return getmostlefttank(root->left, !x);;
            }            
            else if (root->right != NULL && root->left == NULL)
            {
                return getmostlefttank(root->right, !x);
            }
            else
            {
                node* right = getmostlefttank(root->right, !x);
                node* left = getmostlefttank(root->left, !x);

                if (right->tank->position.x < left->tank->position.x) { 
                    //check if the x of right is smaller then left if so right is the most left of the two.
                    return right;
                }
                else
                {
                    return left;
                }
            }
            
        }

    }

    /// <summary>
    /// the sort/update function for the kd tree
    /// </summary>
    /// <param name="root">the </param>
    /// <param name="x">a bool that determines whether the splitting happens on the x or y axis. </param>
    /// <param name="tobesortedchilderen">a pointer to the vector that allows for sorting</param>
    /// <returns>returns the new root of the tree</returns>
    kdTree::node* kdTree::updateinsert(node* root, bool x, vector<node*>* tobesortedchilderen) {

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
}