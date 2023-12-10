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
    /// TODO might be the wrong part
    /// 
    /// </summary>
    /// <param name="list"></param>
    /// <param name="x"></param>
    /// <param name="left"></param>
    /// <param name="rootind"></param>
    /// <param name="parentind"></param>
    /// <param name="i"></param>
    /// <returns></returns>
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

        return median;
    }

    kdTree::node* kdTree::inserttanks(vector<Tank*> list, bool x, bool left, int rootind, int parentind, int i)
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
            else if (left == true) {
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
                medianindex = parentind - ((parentind - rootind) / 2);
            }
        }


        node* median = newnode(list[medianindex]);;

        i = i + 1;

        if ((x == true && parentind - medianindex == 1) || left == true && parentind - medianindex == 1) {
            return median;

        }
        else if (x == false && medianindex - parentind == 1)
        {
            return median;
        }
        else if ((x == true && parentind - medianindex >= 2) || (x == false && medianindex - parentind >= 2) || (x == false && left == true && parentind - medianindex >= 2)) {
            if (x == false) {
                median->left = inserttanks(list, false, true, parentind, medianindex, i);

                median->right = inserttanks(list, false, false, parentind, medianindex, i);
            }
            else {
                median->left = inserttanks(list, true, true, parentind, medianindex, i);

                median->right = inserttanks(list, false, false, parentind, medianindex, i);
            }

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

    vector<kdTree::node*>* kdTree::get_tobe_sortedlist(node* root, vector<node*>* tobesortedchilderen, int i, bool left)
    {
        if (root == NULL) {
            return NULL;
        }


        if (root->left != NULL) {
            node* backup = root->left;
            root->left = NULL;
            i = i + 1;
            tobesortedchilderen = get_tobe_sortedlist(backup, tobesortedchilderen,i,true);
        }

        if (root->right != NULL) {
            node* backup = root->right;
            root->right = NULL;
            i = i + 1;
            tobesortedchilderen = get_tobe_sortedlist(backup, tobesortedchilderen,i,false);
        }

        //cout << i << endl;
        
        if (root->tank->active == true)
        {
            if (tobesortedchilderen->size()<=0) {
                tobesortedchilderen->push_back(root);
                return tobesortedchilderen;
            }

            int maxindex = tobesortedchilderen->size() - 1;


            for (int s = maxindex; s >= 0; s--)
            {
                node* checking_node = tobesortedchilderen->at(s);

                if (root->tank->position.x <= checking_node->tank->position.x) {
                    if (s + 1 > maxindex) {
                        tobesortedchilderen->at(s) = root;
                        tobesortedchilderen->push_back(checking_node);
                        maxindex = tobesortedchilderen->size() - 1;

                    }
                    else
                    {
                        tobesortedchilderen->at(s) = root;
                        tobesortedchilderen->at(s + 1) = checking_node;
                    }
                }
                else {
                    if (s + 1 > maxindex) {
                        tobesortedchilderen->push_back(root);
                    }
                    break;
                }
            }
        }

        return tobesortedchilderen;
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