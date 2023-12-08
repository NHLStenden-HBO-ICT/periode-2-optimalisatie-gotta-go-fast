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

    //todo add the Y insert check
    kdTree::node* kdTree::insertnodes(vector<node*> list, bool x, bool left, int rootind, int parentind, int i)
    {
        int medianindex;
       
        if (x == false) {
            
            if (parentind > rootind) {
                if (left == true) {
                    medianindex = parentind - ((parentind - rootind) / 2);
                }
                else {
                    medianindex = ((parentind - rootind) / 2) + parentind;
                }

            }
            else if(left == true) {
                medianindex = ((parentind - rootind) / 2) + parentind;
            }
            else {
                medianindex = ((rootind - parentind) / 2) + parentind;
            }
        }
        else
        {
            if (parentind <= rootind) {
                medianindex = parentind / 2;
            }
            else
            {
                medianindex = parentind - ((parentind - rootind) / 2) ;
            }
        }




        node* median = list[medianindex];

        i = i + 1;
        
        if ((x==true && parentind- medianindex == 1)|| left == true && parentind - medianindex == 1) {
            return median;

        }
        else if (x == false && medianindex - parentind== 1)
        {
            return median;
        }
        else if((x == true && parentind - medianindex >= 2) || (x == false && medianindex - parentind >= 2) || (x == false&&left == true && parentind - medianindex >= 2)){
            if (x==false) {
                median->left = insertnodes(list, false, true, parentind, medianindex, i);

                median->right = insertnodes(list, false, false, parentind, medianindex, i);
            }
            else {
                median->left = insertnodes(list, true, true, parentind, medianindex, i);

                median->right = insertnodes(list, false, false, parentind, medianindex, i);
            }

        }


        if (i > 100) {
            cout << i << endl;
        }
        return median;
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

        if (root == NULL) {
            return NULL;
        }
        //pow(2.0f, (root->tank->get_position().x - tank.get_position().x)) + pow(2.0f, (root->tank->get_position().y - tank.get_position().y))
        if (root->left != NULL && pow(2.0f, (root->left->tank->get_position().x - tank.get_position().x)) + pow(2.0f, (root->left->tank->get_position().y - tank.get_position().y)) < pow(2.0f, (root->tank->get_position().x - tank.get_position().x)) + pow(2.0f, (root->tank->get_position().y - tank.get_position().y))) {
            if (root->right != NULL && pow(2.0f, (root->right->tank->get_position().x - tank.get_position().x)) + pow(2.0f, (root->right->tank->get_position().y - tank.get_position().y)) < pow(2.0f, (root->left->tank->get_position().x - tank.get_position().x)) + pow(2.0f, (root->left->tank->get_position().y - tank.get_position().y)) ){
                return searchClosest(root->right, tank);
            }
            return searchClosest(root->left, tank);
        }
        //check if right is shortest as long as left is not shorter than root
        else if (root->right != NULL && pow(2.0f, (root->right->tank->get_position().x - tank.get_position().x)) + pow(2.0f, (root->right->tank->get_position().y - tank.get_position().y)) < pow(2.0f, (root->tank->get_position().x - tank.get_position().x)) + pow(2.0f, (root->tank->get_position().y - tank.get_position().y))) {
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
    /// the sort/update function for the kd tree
    /// </summary>
    /// <param name="root">the </param>
    /// <param name="x">a bool that determines whether the splitting happens on the x or y axis. </param>
    /// <param name="tobesortedchilderen">a pointer to the vector that allows for sorting</param>
    /// <returns>returns the new root of the tree</returns>
    kdTree::node* kdTree::updateinsert(node* root, bool x, vector<node*>* tobesortedchilderen) {
        if (root == NULL) {
            return NULL;
        }

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
            //cout << "root is active and needs to be added to list" << endl;
            tobesortedchilderen->push_back(root);
            node* newroot = tobesortedchilderen->at(tobesortedchilderen->size() - 1);

            return tobesortedchilderen->at(tobesortedchilderen->size()-1);
        }

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