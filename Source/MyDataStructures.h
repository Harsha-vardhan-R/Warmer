/*
  ==============================================================================

    MyDataStructures.h
    Created: 11 Mar 2024 9:27:39pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "GraphNodes/Collection.h"
#include <random>


/*

    A dynamic topo sorting data-structure,

    if a new node is added or deleted it will be set in the correct position
    without going through all the nodes every time a node gets added or deleted,

    but `isGood` will be called on all nodes and if any one returned false we are going to
    stop processing the queue and, we still do not support free nodes isolated.

    calling push will insert the node and stop the processing,

    calling the newConnection(GraphNode* from, GraphNode* to) is going to update the position of the
    nodes such that their dependents will get processed first.

    delete automatically takes care of what is needed for processing.(topo sorted list need not be rearranged if a
    node is deleted, we can just remove it without any consequences)


    A lot of the functions in this class are unsafe,
    they do not even check for null, please use with CAUTION

*/



class PriorityQueue : public juce::AudioIODeviceCallback {
public:

    struct linkedNode {
    public:

        GraphNode* nodePointer;
        linkedNode* nextNode = nullptr;

        linkedNode(GraphNode* val) : nodePointer(val) {}
        linkedNode(GraphNode* val, linkedNode* next) : nodePointer(val), nextNode(next) {}

    };


    void processingStart() {
        play.store(1);
    }

    void processingStop() {
        play.store(0);
    }


    linkedNode* headLinkedNode = nullptr;

    PriorityQueue() {
        play.store(0);
    }

    ~PriorityQueue() {
        processingStop();
        /* We are NOT deleting the nodes within the data structure with it,
           but we are deleting the nodes that we created here, `linkedNode`s */
        linkedNode* current = head;
        if (head) recr_delete(current);
    }

    void recr_delete(linkedNode* n) {
        if (n->nextNode) recr_delete(n->nextNode);
        delete n;
    }

    double sampleRate = 0.0, sampleSize = 0.0;

    void setBufferSizeAndRate(double sampleRate, int estimatedSamplesPerBlock) {
        this->sampleRate = sampleRate;
        this->sampleSize = estimatedSamplesPerBlock;
    }

    // Insert an element. O(1)
    void push(GraphNode* nodeGraph) {
        // create and insert a node at the end of the linked list.
        linkedNode* temp = new linkedNode(nodeGraph, head->nextNode);

        // the tail is never going to be null;
        head->nextNode = temp;

        if (!temp->nextNode) tail = temp;

        totalNodesToProcess++;
        bunchOfNodes.insert(nodeGraph);
	}

    // Insert an element. O(1)
    // used to insert all the nodes before sorting them and making a linked-list.
    void pushBlunt(GraphNode* nodeGraph) {
        totalNodesToProcess++;
        bunchOfNodes.insert(nodeGraph);
    }

    // O(n)
	void remove(GraphNode* node) {

        bunchOfNodes.erase(node);

		// search and remove from the linked list,
		// do not delete the node data, it will be done in the Instrument.h "nodeDeleted"
		linkedNode* current = head;

		while (current && current->nextNode->nodePointer == node) {
			current = current->nextNode;
		}

		// remove the node, we have it's previous node.
		linkedNode* toDelete = current->nextNode;
		current->nextNode = current->nextNode->nextNode;

        // if the last node is deleted.
        if (!current->nextNode) {
            tail = current;
        }

        totalNodesToProcess--;

        delete toDelete;
	}

    GraphNode* MIDI_IN = nullptr;


    void setInputNode(InputMasterGraphNode* node) {
        MIDI_IN = static_cast<GraphNode*>(node);
        head = new linkedNode(node);
        tail = head;
        totalNodesToProcess++;
    }

    // calls prepare to play on all the nodes,
    // useful when there are no connections changed but sample rate or size changed.
    void prepare() {
        for (auto i : bunchOfNodes) {
            i->prepareToPlay(sampleRate, sampleSize);
        }
    }

    // returns true if the ZeroDependencyQueue is empty.
    // this means two things :
    //    -> all the nodes are popped
    //    -> some of the nodes dependencies are still being processed.
    // we should not pop in both cases.
    // to check for all nodes popped, use `done()` method.
    //O(1)
    bool isEmpty() {
        return (sortedNodes.size() == 0);
    }

    // Returns true if the nodes are popped.
    //O(1)
    bool done() {
        return (totalNodesToProcess == 0);
    }

    bool checkAllGood() {
        for (auto i : bunchOfNodes) {
            bool b = ((GraphNode*)i)->allGood();
            if (!b) return false;
        }

        return true;
    }


	// stores the head and the tail of the linked list,
	// it is not a double-sided linked list.
	linkedNode* head, *tail;


    bool newConnection(GraphNode *from, GraphNode *to) {

		if (!checkAllGood()) {
            processingStop();
            return false;
        }

        // we call this only when the connections change only on the affected nodes.
        from->prepareToPlay(sampleRate, sampleSize);
        to->prepareToPlay(sampleRate, sampleSize);

		// call the update function pointers for the nodes on each pointer,
		// so it will be triggered for both input and output connections.
		from->setProcessBlockCallback();
		to->setProcessBlockCallback();

		// here we need to make sure that the `from` node is before the `to` node.
		// we can put the `from` just before node and the order will still be valid.

		// start checking for the first node.
		linkedNode* current;
		linkedNode* prev = head;

        current = head;

		int found = 0;
		while (current) {
			// if the `from` node is before to node,
            // they are already in order.
			if (current->nodePointer == from) break;

            // if the `to` node is found first, we are going to set the `from` node,
            // just after the `from` node.
			if (current->nodePointer == to) {

                // if the `from` is not found but to is found that means,
                // our immediate next node can never be a null-pointer because `from` SHOULD be present in the graph.
                linkedNode* temp = current;

                // to also can never be the head, as it is always the first node.
                // so prev will never be nullptr at this place.
                prev->nextNode = current->nextNode;

                // now search for the `from` node which will be present at some point after the
                // `to` node.
                current = current->nextNode;
                while (current && current->nodePointer != from) {
                    current = current->nextNode;
                }

                // now the current is set to the `from` node.
                // we set the `temp`(to) to be immediate next node after the `from`.
                temp->nextNode = current->nextNode;
                current->nextNode = temp;

                // edge case if the `from` node is at the tail.
                if (!temp->nextNode) tail = temp;

                break;
			}

			prev = current;
			current = current->nextNode;
		}

        debugDump();

        processingStart();
        return true;
    }

    std::atomic<float> level;

    void setVolumeLevel(float val) {
        level.store(val);
    }

    bool connectionRemoved(GraphNode *from, GraphNode *to) {

        processingStop();

        // call the update function pointers for the nodes on each pointer,
        // so it will be triggered for both input and output connections.
        from->setProcessBlockCallback();
        to->setProcessBlockCallback();

        // if an important connection is removed we will stop processing.
        if (!checkAllGood()) return false;

        // else continue
        return true;
    }

    // this is used while loading a configuration,
    // as loading an instrument will be really slow if
    bool sort() {

        if (MIDI_IN == nullptr) return false;

        std::unordered_map<GraphNode*, std::set<GraphNode*>> AdjList;
        std::unordered_map<GraphNode*, int> indegree;  // In-degree counter for each node
        std::queue<GraphNode*> zeroDependencyQueue;    // Queue for nodes with zero dependencies

        if (!checkAllGood()) return false;

        // Prepare the Adjacent List and in-degree for each node
        for (auto i : bunchOfNodes) {
            AdjList[i] = (i)->getDependents();
            indegree[i] = 0;
        }

        // Calculate in-degree for all nodes based on AdjList
        for (auto& [node, dependents] : AdjList) {
            for (auto dependent : dependents) {
                if (!indegree.count(dependent)) {
                    std::cout << dependent << " is not present in the map, How??, dependent of : " << node->getName() << "\n";
                }
                indegree[dependent]++;
            }
        }

        // If 0 in-degree, we can process them in any order.
        for (auto& [node, degree] : indegree) {
            if (degree == 0) {
                zeroDependencyQueue.push(node);
            }
        }

        // because this is not in the AdjList, we have to do this manually.
        sortedNodes.push_back(MIDI_IN);
        for (auto i : (MIDI_IN)->getDependents()) {
            indegree[i]--;
            if (indegree[i] == 0) {
                zeroDependencyQueue.push(i);
            }
        }

        // Topological sort
        while (!zeroDependencyQueue.empty()) {
            auto currentNode = zeroDependencyQueue.front();
            zeroDependencyQueue.pop();
            sortedNodes.push_back(currentNode);  // Add current node to sorted list

            // Reduce the in-degree of adjacent nodes by 1.
            for (auto dependent : AdjList[currentNode]) {
                indegree[dependent]--;
                if (indegree[dependent] == 0) {
                    zeroDependencyQueue.push(dependent);
                }
            }
        }

        if (sortedNodes.size() != bunchOfNodes.size()+1) {
            sortedNodes.clear();
            return false;
        }

        // now create the linked-list of the processing order based on this sorted vector.
        // we obviously have the first node as the MIDI_IN
        // which should be set before calling this sort function.
        if (head) recr_delete(head);

        linkedNode* current = new linkedNode(MIDI_IN);
        head = current;
        // starting from 1 because at zero we will always have MIDI_IN, that we already inserted.
        for (int i = 1; i < sortedNodes.size(); ++i, current = current->nextNode) {
            current->nextNode = new linkedNode(sortedNodes[i]);
            tail = current->nextNode;
        }

        debugDump();

        return true;  // No cycle
    }


    std::vector<GraphNode*> getTopoSortedNodes() {
        return sortedNodes;
    }

    // Clears all the containers.
    void flush() {
        sortedNodes.clear();
        bunchOfNodes.clear();
    }

    // for debugging.
    void debugDump() {
        std::cout << "Elements in Queue : " << "\n";
        recrPrint(head);
    }

    void recrPrint(linkedNode* node) {
        if (node) {
            std::cout << node->nodePointer->name << "\n";
            recrPrint(node->nextNode);
        }
    }

    std::vector<GraphNode*> sortedNodes;
    std::set<GraphNode*> bunchOfNodes;



    // ======================================
    // METHODS FOR AUDIO DEVICE IO CALLBACK.
    // ======================================

    void generateWhiteNoise(float* outputChannel, int numSamples, float l)
    {
        // Create a random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f); // Generate values between -1 and 1

        // Generate white noise samples
        for (int i = 0; i < numSamples; ++i)
        {
            outputChannel[i] = dis(gen)*l;
        }
    }

    void audioDeviceIOCallbackWithContext ( const float* const* inputChannelData,
                                            int numInputChannels,
                                            float* const* outputChannelData,
                                            int numOutputChannels,
                                            int numSamples,
                                            const juce::AudioIODeviceCallbackContext& context ) override {

        if (!play.load()) {
            for (int channel = 0; channel < numOutputChannels; ++channel)
            {
                std::fill(outputChannelData[channel], outputChannelData[channel] + numSamples, 0.0f);
            }
            return;
        }

        float l = level.load();

        for (int channel = 0; channel < numOutputChannels; ++channel)
        {
            float* outputChannel = outputChannelData[channel];
            generateWhiteNoise(outputChannel, numSamples, l);
        }

    }

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override
    {
        // Initialization code when audio device is about to start
    }

    void audioDeviceStopped() override
    {
        // Cleanup code when audio device is stopped
    }

private:

    std::atomic<int> play;

    int totalNodesToProcess;

};
