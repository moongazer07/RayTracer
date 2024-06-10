

Here we sketch the algorithm and the data structures for bvh.

First, let us write the general idea:

0. Create a structure for keeping all objects there:

    - Spheres
    - Triangles

    HOW TO DO IT?

1. Put every element of the scene inside a box
    
    How to compute a box? 

    Create a structure Box, 
    that will keep two vectors:
    bottom left nearest point
    top right furthers point

    Create a methods for it: 
    
    update the box, which will take
    min and max points and then update the values
    Primarily the vlues inside let be equal to inf

    find the box (triangle)

    build the box (sphere)

    build the box (node)



    We have three normals { (1, 0, 0) -- normal for y, z axis plane 
                            (0, 1, 0) -- normal for x, z axis plane
                            (0, 0, 1) -- normal for x, y axis plane
                           }

        Triangle:

            we want to find the distance for every point, 
            how far is it from the origin of the coordinates. 

    Say if we want to find the distance from the point projection on
    the plane (Y,Z), we have to use (1,0,0) normal

    We will need the lowest point and the highest point:

        Lowest: (min(d_x_1, d_x_2, d_x3)
                    ...
                min(d_z_1, d_z_2, d_z_3))

        Same for Highest.


        Sphere:

    To compute the box for the sphere having its center and
    its radius -- take the center:

        Lowest: ((x_c - rad, y_c - rad, z_c - rad))
        Highest: ((x_c + rad, y_c + rad, z_c + rad))

    
2. Start building a tree

    Tree consists of: 
    
        Node root
        int depth

    Node consists of:

        bool    --> leaf
        Vector  --> children
        Vector  --> Objects in the node
        double  --> Distance from the origin ray to the box
        Vector  --> point_min
        Vector  --> point_max
        Vector  --> center  

    1) Compute the largest box (the root of the tree) -- IN FACT
    CAN BE DONE ON THE PREVIOUS STEP SO THAT WE DON'T HAVE TO GO THROGH THE
    ARRAY TWICE
    
    - it includes all the boxes 
    - For this you need to go through all the objects, and 
    find the max of all coordinates, and min of all coordinates
    - Min and Max will be respectively the bottom left nearest point and
    up right further point of the global box 

    2) Start builing elements in the tree

        For each Object we have to find in which node it will be:

        Having a root, and an object, how to find in which node the
        Object should be?

            - Depends on:

                - If the node is a leaf
                - If the node has any other objects
                - If the node is on max depth of the tree
                - Where is the centroid of object and centroid of parent

            if (node is leaf) 

                if (node is empty OR we are at max depth) 

                    insert the provided object to the node's data

                else 

                    In this case we have to say that node is not
                    leaf anymore

                    And we have to split the node in 8 equal parts
                    For this we need a center of the node, and we
                    shell fill the vector of children with new nodes

                    We will put the objects inside our new children
                    nodes

                    And insert the same object into this parent node again

            else (node is not a leaf) 

                This means that we will have to find the right child node for the
                object to insert it to

                Find the center for the node

                Loop through all the children of the node

                    There are 8 children

                    Depending on the position of centroid towards 
                    the centroid of the node -- choose the index for the object where
                    to insert element

                    Call the recursion again on the chosen child of the node


    3) Once the tree is build we can update the bounds of the nodes:

        - Instead making them all of the same size, we will
        change the sizes depending on the sizes of the boxes of their children

        - The size of a leaf node is computed as the bigest box of all elements
        inside the box

    
3. With the tree we can create new rendering algorithms

    - Essentially the largest change is in the intersection function

    - Now we have to find the leaf that the box that the ray intersects

    Having a ray and a tree: 

        call Intersect(ray, tree->root);

        i. Compute wheather the ray intersects the root box

        ii. if it does:: create a queue and put a node there

        iii. Then we shall operate until the queue is empty

        iv. while (!queue.empty()) 

            Take the node

            Pop it from the queue

            if (is leaf)

                Check for all the objects wheather they are intersected (can use 
                the original function for finding intersection)

                Record the smallest distance from there

                This distance will be used in order to check the intersection with other nodes,
                i.e. if the distance to the node is smaller than the distance to the object --
                you don't have to count the distance to the objects in the node

            else 

                i.e. if it is not a leaf:

                - go through the children if this node
                - compute the distance to each of them
                - if the distance is smaller than the smallest distance so far --
                add the node to the qeueu

        By the end of this process you will have the result of intersection stored (or not)
        So you can compute the color. 


    We also need a function to compute the distance to the nodes:

        IntersectBox(ray, node, tNear, tFar) 

        -- observe that we need tNear, tFar in order to update them in case we find larger
        tn and smaller tf


            For every ray there is precomputed numerator and denominator: 
                -   numerator = Normal \dot Origin
                -   denominator = normal \dot Ray

            We need them in order to compute the distance towards the volume

                We have Dnear, Dfar which represent the nearest and furtherest
                projection for the element.

                Using the Dnear and Dfar we can find the distance from the ray
                to the plane with the corresponding normal.

                Taking the largest tNear we will obtain the nearest distance, and
                taking the smallest tFar we will obtain the furthest distance

                Having these distances we will be able to order the boxes in the priority queue
                using min heap operator

                Cases to look after:

                    If denominator < 0 => swap tf and tn
                    If tn > tNear -- update tNear
                    if tf < tFar -- update tFar
                    if (tNear > tFar) return false

                return true 

        We will use the priority queue to order nodes on the tree
        for checking with the ray intersection
        
        Questions:

        how do we use tNear, tFar? 
        how do we update the distance towards the new box?
        how do we update the distance towards the new object and is it correlated with the distance to
        boxes?

            