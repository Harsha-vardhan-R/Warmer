/*
  ==============================================================================

    MyDataStructures.h
    Created: 11 Mar 2024 9:27:39pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "GraphNodes/GraphNode.h"

/*
    Contains my implementation of a Priority Queue.

    Cannot use the STL std::priority_queue because,
    it does not support updating priority of the elements already in it.

    Technically this is not exactly a PriorityQueue, named it because it does the same thing with some extra features.

    ***
    This is NOT designed to add new nodes continuously.
    you are meant to add only before calling the pop and init.

    A normal use would be like:
        -> `push` all the wanted nodes.
       /-> use `poll' to return 0 dependent nodes.
      | -> update nodes dependency.
      | -> use `init` to go to the initial state (technically this takes linear time , but in practical applications it is mostly constant time).
      | -> go to point 2 ->\
      \____________________/
   ***
*/

class PriorityQueue {
public:

    PriorityQueue() {
//        ZeroDependentQueue = new std::queue<GraphNode*>();
//        elementDependencyMap = new std::unordered_map<GraphNode*, int>();
//        InitialZeroDependents = new std::vector<GraphNode*>();
        totalNodesToProcess = 0;
    }

    ~PriorityQueue() { /* We are NOT deleting the nodes within the data structure with it */ }

    // Insert an element. O(1)
    void push(GraphNode* node) {
        if (node->permDependency == 0) {
            InitialZeroDependents.push_back(node);
            ZeroDependentQueue.push(node);
        } else {
            elementDependencyMap[node] = node->permDependency;
        }
        totalNodesToProcess++;
    }

    // returns true if the ZeroDependencyQueue is empty.
    // this means two things :
    //    -> all the nodes are popped
    //    -> some of the nodes dependencies are still being processed.
    // we should not pop in both cases.
    // to check for all nodes popped, use `done()` method.
    //O(1)
    bool isEmpty() {
        return (ZeroDependentQueue.empty());
    }

    // Returns true if the nodes are popped.
    //O(1)
    bool done() {
        return (totalNodesToProcess == 0);
    }

    // pop's the element from the queue with zero dependencies and returns it, if the queue is empty it crashes.
    // unsafe , use only after checking if the queue is empty.
    // O(1)
    GraphNode* pop() {
        auto front = ZeroDependentQueue.front();
        ZeroDependentQueue.pop();
        totalNodesToProcess--;
        return front;
    }

    // Returns the top(most priority) node does not pop it.
    // O(1)
    // you do not need this method for normal usage, because the elements popped are guaranteed to have 0 dependents.
    GraphNode* top() {
        return ZeroDependentQueue.front();
    }

    // init : goes to the initial state before any pop.
    void init() {
        for (GraphNode* i : InitialZeroDependents) ZeroDependentQueue.push(i);
        totalNodesToProcess = elementDependencyMap.size()+InitialZeroDependents.size();
    }

    // decreases the value of a node by one in the hash map, if the nodes dependencies become 0 it gets pushed to
    // the queue where it is popped when the pop method is called.
    // if the node is not present it will be created leading to undefined behaviour. so always
    // make sure you know the node actually exists in the map.
    void update(GraphNode* node) {
        elementDependencyMap[node] -= 1;

        if (elementDependencyMap[node] == 0) {
            // update the node's dependencies for the next call.
            elementDependencyMap[node] = node->permDependency;
            ZeroDependentQueue.push(node);
        }
    }

    // for debugging, data structure becomes useless after this, you need to build again.
    void debugDump() {
        std::cout << "Elements in Queue : " << "\n";
        while (!ZeroDependentQueue.empty()) {
            std::cout << ZeroDependentQueue.front()->name << "\n";
            ZeroDependentQueue.pop();
        }
        std::cout << "Elements in Map : " << "\n";
        for (auto i : elementDependencyMap) {
            std::cout << i.first << i.second << "\n";
        }
    }

private:

    // underlying data structures.
    std::queue<GraphNode*> ZeroDependentQueue;
    std::unordered_map<GraphNode*, int> elementDependencyMap; // Stores the dependencies for the present iteration.
    std::vector<GraphNode*> InitialZeroDependents;

    int totalNodesToProcess;

};