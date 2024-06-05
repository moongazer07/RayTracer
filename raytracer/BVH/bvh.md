

Here we sketch the algorithm and the data structures for bvh.

First, let us write the general idea:

0. Create a structure for keeping all objects there:

    - Spheres
    - Triangles

    HOW TO DO IT?

1. Put every element of the scene inside a box
    
    How to compute a box? 

    We have three normals { (1, 0, 0) -- normal for y, z axis plane 
                            (0, 1, 0) -- normal for x, z axis plane
                            (0, 0, 1) -- normal for x, y axis plane
                            }

        Triangle:

            we want to find the distance for every point, 
    h       how far is it from the origin of the coordinates. 

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
                - Wheather the box of the object intersects the nodes' box

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

        We will use the priority queue to order nodes on the tree
        for checking with the ray intersection

        

            