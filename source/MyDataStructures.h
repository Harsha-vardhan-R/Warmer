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
#include <chrono>
#include "WaveShapeDisp.h"
#include "juce_audio_basics/juce_audio_basics.h"

/*

    A dynamic topo sorting data-structure,

    if a new node is added or deleted it will be rearranged to the correct position
    without going through all the nodes every time a node gets added or deleted,

    but `isGood` will be called on all nodes and if any one returned false we are going to
    stop processing the queue and, we still do not support free nodes isolated(this is also true for saving
    the configuration).

    calling push will insert the node and stop the processing,

    calling the newConnection(GraphNode* from, GraphNode* to) is going to update the position of the
    nodes such that their dependents will get processed first.

    delete automatically takes care of what is needed for processing.(topo sorted list need not be rearranged if a
    node is deleted, we can just remove it without any consequences)


    A lot of the functions in this class are unsafe,
    they do not even check for null, please use with CAUTION



    This is also the class that manages a pool of audio-buffers,
    that are distributed to the graph Nodes to be overwritten,
    the nodes take input from the connections witch will be set from the previous node.

*/

struct linkedNode {
public:

    GraphNode* nodePointer;
    linkedNode* nextNode = nullptr;

    explicit linkedNode(GraphNode* val) : nodePointer(val) {}
    linkedNode(GraphNode* val, linkedNode* next) : nodePointer(val), nextNode(next) {}

};

class TopoSorter : public juce::AudioIODeviceCallback {
public:

    // will be true if the callback function is still running,
    // used for manual synchronisation.
    // by inside callback we mean after the pause statement to the end of the method.
    // this is to know we are not processing any nodes, because we edit their configurations from the GUI
    // thread.
    std::atomic<bool> insideCallback;

    void processingStart() {
        play.store(true);
    }

    // This function sets the ply atomic variable to false,
    // which fills the output buffers with size zero, so we do not hear anything.
    // ** IMPORTANT **
    // this function also waits till all the graph nodes are processed,
    // for the current callback (variable play being false is guaranteed to stop processing the
    // nodes for the callback but this call back may still be processing nodes so we wait till this completes).
    void processingStop() {
        play.store(false);
        while (insideCallback.load()); // infinite loop until we are not inside the callback,
        // processing nodes, now it is safe to delete nodes and such,
        // note that if there was an infinite loop in a node while processing and this is called,
        // it may cause undefined behaviour (freeze or a crash).
    }

    linkedNode* headLinkedNode = nullptr;

    TopoSorter() {
        play.store(false);
        insideCallback.store(false);
    }

    ~TopoSorter() override {
        insideCallback.store(false);

        processingStop();
        /* We are NOT deleting the nodes within the data structure with it,
           but we are deleting the nodes that we created here, `linkedNode`s */
        linkedNode* current = head;

        for (juce::AudioBuffer<float>* ab : bunchOfBuffers) { delete ab; }
        for (juce::AudioBuffer<float>* ab : bunchOfBuffersForFeedbacks) { delete ab; }

        if (head) recr_delete(current);
    }

    void recr_delete(linkedNode* n) {
        if (n->nextNode) recr_delete(n->nextNode);
        delete n;
    }

    double sampleRate = 0.0, sampleSize = 0.0;

    void setBufferSizeAndRate(double sampleRate, int estimatedSamplesPerBlock) {

        processingStop();

        // we need to delete and make new buffers only if the number of samples in each
        // block is changed.
        if (this->sampleSize != estimatedSamplesPerBlock) {
            std::queue<juce::AudioBuffer<float>*> audioBuffersToDelete;

            for (auto i : bunchOfBuffers) {
                audioBuffersToDelete.push(i);
            }

            bunchOfBuffers.clear();

            // we can just delete all the buffers now,
            // as we are sure the nodes are not being processed. (check out `processingStop`)
            while(!audioBuffersToDelete.empty()) {
                juce::AudioBuffer<float>* temp = audioBuffersToDelete.front();
                audioBuffersToDelete.pop();
                delete temp;
            }

            this->sampleRate = sampleRate;
            this->sampleSize = estimatedSamplesPerBlock;

            prepare();

            setBuffersToNodes();

            resetAll();
        } else if (this->sampleRate != sampleRate) {
            this->sampleRate = sampleRate;

            prepare();

            // we need not set buffers as the number of samples in a block
            // did not change.

            resetAll();
        }

        if (checkAllGood()) processingStart();

    }

    // Insert an Graph Node. O(1)
    void push(GraphNode* nodeGraph) {
        processingStop();

        linkedNode* tm;

        if (head) { // put it after the head.
            linkedNode* temp = new linkedNode(nodeGraph, head->nextNode);
            head->nextNode = temp;
            tm = temp;
        } else {
            head = new linkedNode(nodeGraph, nullptr);
            tm = head;
        }

        if (!tm->nextNode) tail = tm;

        totalNodesToProcess++;
        bunchOfNodes.insert(nodeGraph);

        nodeGraph->prepareToPlay(sampleRate, sampleSize);

        if (checkAllGood()) processingStart();
	}

    // Insert an element. O(1)
    // used to insert all the nodes before sorting them and making a linked-list of linkedNode's.
    void pushBlunt(GraphNode* nodeGraph) {
        totalNodesToProcess++;
        nodeGraph->prepareToPlay(sampleRate, sampleSize);
        bunchOfNodes.insert(nodeGraph);
    }

    // O(n)
	void remove(GraphNode* node) {
        processingStop();

        bunchOfNodes.erase(node);

		// search and remove from the linked list,
		// do not delete the node data, it will be done in the Instrument.h "nodeDeleted"
        // here we delete the linkedNode associated with the nodePointer.
		linkedNode* current = head;
        linkedNode* prev = nullptr;

        // start searching for the node, stop when the node is found and is in the variable current.
        while (current && current->nodePointer != node) {
            prev = current;
            current = current->nextNode;
        }

        if (current) {
            // the node to remove is not the first node.
            if (prev) {
                linkedNode* temp = current;
                prev->nextNode = current->nextNode;
                delete temp;
                if (!prev->nextNode) tail = prev;
            } else {
                linkedNode* temp = current;
                head = current->nextNode;
                delete temp;
                if (!head->nextNode) tail = head;
            }
        } else {
            std::cout << "ERROR : The node you are trying to remove does not exist in the processing list" << "\n";
        }

        totalNodesToProcess--;

        setBuffersToNodes();

        resetAll();

        if (checkAllGood()) processingStart();
	}

    GraphNode* MIDI_IN = nullptr;
    GraphNode* AUDIO_OUT = nullptr;

    // called to put the MIDI_IN in 1st position.
    void setInputNode(InputMasterGraphNode* node) {
        MIDI_IN = static_cast<GraphNode*>(node);
        totalNodesToProcess++;
    }

    void setOutputNode(OutputMasterGraphNode* nodePointer) {
        AUDIO_OUT = static_cast<GraphNode*>(nodePointer);
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
    [[nodiscard]] bool isEmpty() const {
        return (sortedNodes.empty());
    }

    // checks if every node in the bunch of nodes has isMust sockets connected.
    bool checkAllGood() {
        for (auto i : bunchOfNodes) {
            bool b = ((GraphNode*)i)->allGood();
            if (!b) return false;
        }

        return true;
    }


	// stores the head and the tail of the linked list,
	// it is not a double-sided linked list.
	linkedNode* head = nullptr, *tail = nullptr;
    // manually created and deleted.
    std::set<juce::AudioBuffer<float>*> bunchOfBuffers;
    std::set<juce::AudioBuffer<float>*> bunchOfBuffersForFeedbacks;

    bool newConnection(GraphNode *from, GraphNode *to) {
        processingStop();

        std::set<GraphNode*> visitedSet;

		// here we need to make sure that the `from` node is before the `to` node.
		// we can put the `from` just before node and the order will still be valid.
        bool connectionValid = recrSortChildren(from, to, visitedSet);

        setBuffersToNodes();
        resetAll();
        if (connectionValid && checkAllGood()) processingStart();

        // debugDump();

        return true;
    }

    bool recrSortChildren(GraphNode* from, GraphNode* to, std::set<GraphNode*>& visitedNodes) {

        if (!from || !to) {
            std::cout << "Null pointers passed as arguments in TopoSorter::newConnection, ERROR" << "\n";
            return false;
        }

        bool flag = true;

        if (visitedNodes.find(to) != visitedNodes.end()) {
            std::cout << "Loop Detected in the configuration please remove it, loop part of the loop : "
                      << from->name << "->" << to->name << "\n";
            processingStop();
            flag = false;
        }

        // start checking for the first node.
        linkedNode *current = head;
        linkedNode *prev = nullptr;

        while (current) {

            // if the `from` node is found first.
            if (current->nodePointer == from) break;

            // if the `to` node is found first, we are going to set the `from` node,
            // just after the `from` node.
            if (current->nodePointer == to) {

                // store the pointer to the `to` node.
                linkedNode *temp = current;

                if (prev) prev->nextNode = current->nextNode;
                else head = current->nextNode;

                // now search for the `from` node which will be present at some point after the
                // `to` node, if from is found the current will be set to the linkedNode containing `from`.
                current = current->nextNode;
                while (current && current->nodePointer != from) {
                    current = current->nextNode;
                }
                
                // we found `from`
                if (current) {
                    // now the current is set to the `from` node.
                    // we set the `temp`(to) to be immediate next node after the `from`.
                    temp->nextNode = current->nextNode;
                    current->nextNode = temp;
                } else {
                    std::cout << "From node not found in TopoSorter::newConnection, this should not happen, ERROR"
                              << "\n";
                }

                // edge case if the `from` node is at the tail.
                // this should not happen(AUDIO OUT is always the last), but in case.
                if (!temp->nextNode) tail = temp;

                break;
            }

            prev = current;
            current = current->nextNode;
        }

        visitedNodes.insert(from);

        // recursively sort the nodes that are connected to this.
        for (auto i : to->getDependents()) flag = flag && recrSortChildren(to, i, visitedNodes);

        return flag;

    }


    // !! Should be called only after setting the buffers,
    // and putting the nodes in topo sorted order.
    void resetAll() {
        linkedNode* current = head;

        (static_cast<InputMasterGraphNode*>(MIDI_IN))->setInputMIDI(&midiBuffer);


        while (current) {
            current->nodePointer->reset();
            current = current->nextNode;
        }

        if (WaveShapeDisp::this_instance) WaveShapeDisp::this_instance->changeBufferSize((int)sampleSize);

        (static_cast<OutputMasterGraphNode*>(AUDIO_OUT))->reset();

    }

    std::atomic<float> level;

    void setVolumeLevel(float val) {
        level.store(val);
    }

    juce::AudioBuffer<float>* getOutputWave() {

        if (AUDIO_OUT) return AUDIO_OUT->getToWriteAudioBuffer();
        else return nullptr;
    }

    // Returns if the nodes are all good and can be processed,
    // also stops the processing if that is not the case.
    bool connectionRemoved(GraphNode *from, GraphNode *to) {

        processingStop();

        // if an important connection is removed we will stop processing.
        if (!checkAllGood()) return false;

        // call the update function pointers for the nodes on each pointer,
        // so it will be triggered for both input and output connections.
        from->reset();
        to->reset();

        processingStart();

        // else continue
        return true;
    }

    int num_of_buffers_used = 0;


    void setBuffersToNodes() {

//        std::set<juce::AudioBuffer<float>*> totSetHere;

        // Now take the linkedList and give it AudioBufferPointers! to write.
        // this is not the most efficient way per se, but it is miles better than giving an
        // individual AudioBuffer to each and every node that needs it.
        //
        // we start considering from the first node.
        linkedNode* current = head;
        // stores the number of nodes that are not processed but need this to process.
        std::unordered_map<juce::AudioBuffer<float>*, int> dependentCountMap;
        std::queue<juce::AudioBuffer<float>*> dependencyFreedBuffers;
        std::unordered_map<GraphNode*, std::set<juce::AudioBuffer<float>*>> nodeToDependentBufferMap;

        // if there are any left-over buffers.
        // we never delete the buffers in between (exception : changing the number of samples in each block)
        // execution so for an instance the maximum number of buffers will be
        // the maximum of buffers that are needed for the configuration that took the max amount of unique buffers.
        //
        // after every connection we will set the audio buffers,
        // may look inefficient, but still coming up with better ideas.
        // I have a dumb solution but i'm pretty sure it will deviate far from optimised path when
        // adding connections in a certain way, but this surely works for all the cases.
        // best solution cannot be guaranteed as topo sort can have multiple solutions.
        for (auto i : bunchOfBuffers) {
            dependencyFreedBuffers.push(i);
        }

        if (!checkAllGood()) return;

        while (current) {
            GraphNode* currentNode = current->nodePointer;

            if (currentNode->needAudioBuffer) {

                // if there are no free buffers we create one.
                if (dependencyFreedBuffers.empty()) {
                    auto* t = new juce::AudioBuffer<float>(2, (int)std::ceil(sampleSize));
                    dependencyFreedBuffers.push(t);
                    bunchOfBuffers.insert(t);
                    num_of_buffers_used++;
                }

                juce::AudioBuffer<float>* temp = dependencyFreedBuffers.front();
                dependencyFreedBuffers.pop();
//                totSetHere.insert(temp);

                // we give this buffer that we just popped to the GraphNode.
                currentNode->setToWriteAudioBuffer(temp);

                // we cannot reuse this buffer unless all the buffer dependencies to it get
                // processed.
                //
                // and the dependentCountMap kind of holds the number of dependencies for this
                // current buffer.
                for (GraphNode* i : currentNode->getAudioBufferDependencies()) {
                    // this hash map stores the
                    // node -> a set of all the buffers it needs to get processed.
                    // the set will keep on increasing if the node is getting modulated from different
                    // other outputs.
                    nodeToDependentBufferMap[i].insert(temp);
                    dependentCountMap[temp]++;
                }

                // recycle the node's buffers, whose dependents are processed(we will not be reading that anymore for this block).
                // instead of leaving it, we assign it to another node.
                // now this node is processed, so all the buffers it was dependent on should get their
                // reference count reduced.
                for (auto i : nodeToDependentBufferMap[currentNode]) {
                    dependentCountMap[i]--;

                    // this node can be used, all it's dependencies are processed.
                    // if the node's buffer cannot be reused then we are not going to
                    // free it.
                    if (dependentCountMap[i] == 0) dependencyFreedBuffers.push(i);
                }

                nodeToDependentBufferMap.erase(currentNode);

            }

            current = current->nextNode;
        }

//        debugDump();

//        std::cout << "Number of Buffers used for this configuration : " << totSetHere.size() << "\n";

    }

    void clearAll() {
        if (head) recr_delete(head);
        head = nullptr;
        bunchOfNodes.clear();
        bunchOfNodes.insert(MIDI_IN);
        bunchOfNodes.insert(AUDIO_OUT);
    }

    // this is used while loading a configuration,
    // as loading an instrument will be really slow if it is 
    // dynamically topo sorted for every connection.
    bool sort() {
        processingStop();
        prepare();
        sortedNodes.clear();

        if (head) recr_delete(head);
        head = nullptr;

        if (!MIDI_IN || !AUDIO_OUT) {
            std::cout << "FATAL! MIDI IN OR AUDIO OUT ARE NOT PRESENT IN THE NODE SET" << "\n";
            return false;
        }

        std::cout << "All the nodes in BunchOfNodes : " << '\n';
        for (auto i : bunchOfNodes) {
            std::cout << i->name << "\n";
        }


        bunchOfNodes.insert(MIDI_IN);
        bunchOfNodes.insert(AUDIO_OUT);

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



        // Topological sort
        while (!zeroDependencyQueue.empty()) {
            auto currentNode = zeroDependencyQueue.front();
            zeroDependencyQueue.pop();
            sortedNodes.push_back(currentNode);  // Add current node to sorted list

            // Reduce the in-degree of adjacent nodes by 1.
            for (auto dependent: AdjList[currentNode]) {
                indegree[dependent]--;
                if (indegree[dependent] == 0) {
                    zeroDependencyQueue.push(dependent);
                }
            }
        }

        if (sortedNodes.size() != bunchOfNodes.size()) {
            std::cout << "Not all nodes satisfied in sort in TopoSorter, probably a Loop is present" <<"\n";
        }

        auto found = std::find(sortedNodes.begin(), sortedNodes.end(),MIDI_IN);

        if (found == sortedNodes.end()) {
            std::cout << "ERROR!, MIDI_IN not found in SortedNodes after sorting" << "\n";
            return false;
        } else {
            sortedNodes.erase(found);
        }

        auto found1 = std::find(sortedNodes.begin(), sortedNodes.end(), AUDIO_OUT);

        if (found1 == sortedNodes.end()) {
            std::cout << "ERROR!, AUDIO_OUT not found in SortedNodes after sorting" << "\n";
            return false;
        } else {
            sortedNodes.erase(found1);
        }

        auto *current = new linkedNode(MIDI_IN);
        head = current;
        for (int i = 0; i < sortedNodes.size(); ++i, current = current->nextNode) {
            current->nextNode = new linkedNode(sortedNodes[i]);
            tail = current->nextNode;
        }

        setBuffersToNodes();

        resetAll();

        if (checkAllGood()) processingStart();

        // If we are in juce debug, print the debug dump.
        #if JUCE_DEBUG 
            debugDump();
        #endif
        
        return true;
    }


    std::vector<GraphNode*> getTopoSortedNodes() const {
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
            std::cout << node->nodePointer->name << ", Pointer value : " << node->nodePointer << ", Buffer used : "
                << node->nodePointer->getToWriteAudioBuffer() << "\n";
            recrPrint(node->nextNode);
        }
    }

    std::vector<GraphNode*> sortedNodes;
    std::set<GraphNode*> bunchOfNodes;

    // when we are using feedback loops it
    // rearranges the order in which we need to reset the nodes,
    // if there are no directed loops because all the nodes are topo sorted there will
    // not be any kind of situations where we are reading the set values before
    // we set them. but when we have a loop the values(setBufferPointer())
    // will be set after reading. so to make the node work properly we reset the nodes
    // that are the connected from feedback nodes again, int the resetAll method.
    std::set<GraphNode*> nodesToRestTwice;


    // ======================================
    // METHODS FOR AUDIO DEVICE IO CALLBACK.
    // ======================================

    juce::MidiBuffer midiBuffer;
    void setNextMidiBuffer(juce::MidiBuffer& buffer); // sets the next midi-buffer in midiBuffer.

    void printMidiBuffer(const juce::MidiBuffer& midiBuffer) {

        for (const auto metadata : midiBuffer) {
            const auto& message = metadata.getMessage();
            const int samplePosition = metadata.samplePosition;

            if (message.isNoteOn())
            {
                int noteNumber = message.getNoteNumber();
                int velocity = message.getVelocity();
                std::cout << "Note On: Note=" << noteNumber
                          << " Velocity=" << velocity
                          << " Sample Position=" << samplePosition << std::endl;
            }
            else if (message.isNoteOff())
            {
                int noteNumber = message.getNoteNumber();
                std::cout << "Note Off: Note=" << noteNumber
                          << " Sample Position=" << samplePosition << std::endl;
            }
        }

    }

    // +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
    // The main callback function that is called by the device manager in a very high priority thread,
    // this processes all the nodes in order and copies the data in to the output buffer that is going to get played back.
    void audioDeviceIOCallbackWithContext ( const float* const* inputChannelData,
                                            int numInputChannels,
                                            float* const* outputChannelData,
                                            int numOutputChannels,
                                            int numSamples,
                                            const juce::AudioIODeviceCallbackContext& context ) override {

//        auto start = std::chrono::high_resolution_clock::now();
//        std::cout << "reached" << "\n";

//        setNextMidiBuffer();


        // if we are paused, fill it with 0's.
        // you can remove the callback, but this is very inexpensive so not a problem.
        if (!play.load()) {
            juce::AudioBuffer<float> buffer (outputChannelData, numOutputChannels, numSamples);
            buffer.clear();
//            printMidiBuffer(midiBuffer);
            midiBuffer.clear();
            return;
        }


        insideCallback.store(true);

        // Process all the nodes in the list.
        linkedNode* current = head;
        // we process all the nodes
        while (current) {
            current->nodePointer->processGraphNode();
            current = current->nextNode;
        }

        // this output node takes in output channel data and the loudness level.
        // it basically copies the buffer multiplied with the loudness level.
        // ((OutputMasterGraphNode*)AUDIO_OUT)->processGraphNodeOutput(outputChannelData, level.load());
        WaveShapeDisp::this_instance->pushFreshBuffer(((OutputMasterGraphNode*)AUDIO_OUT)->getBufferToDisplay());

//        auto end = std::chrono::high_resolution_clock::now();
//        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//        std::cout << "Execution time: " << duration.count() << " microseconds" << numSamples << std::endl;

        insideCallback.store(false);

//        printMidiBuffer(midiBuffer);
        midiBuffer.clear();

    }

    void processMidiGiveAudio (juce::MidiBuffer& buffMidi, juce::AudioBuffer<float>& outputChannelData) {

//        auto start = std::chrono::high_resolution_clock::now();
//        std::cout << "reached" << "\n";
        setNextMidiBuffer(buffMidi);

        if (!play.load()) {
            outputChannelData.clear();
//            printMidiBuffer(midiBuffer);
            midiBuffer.clear();
            return;
        }


        insideCallback.store(true);

        // Process all the nodes in the list.
        linkedNode* current = head;
        // we process all the nodes
        while (current) {
            current->nodePointer->processGraphNode();
            current = current->nextNode;
        }

        // this output node takes in output channel data and the loudness level.
        // it basically copies the buffer multiplied with the loudness level.
        ((OutputMasterGraphNode*)AUDIO_OUT)->processGraphNodeOutput(outputChannelData, level.load());
        WaveShapeDisp::this_instance->pushFreshBuffer(((OutputMasterGraphNode*)AUDIO_OUT)->getBufferToDisplay());

//        auto end = std::chrono::high_resolution_clock::now();
//        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//        std::cout << "Execution time: " << duration.count() << " microseconds" << numSamples << std::endl;

        insideCallback.store(false);

//        printMidiBuffer(midiBuffer);
        midiBuffer.clear();

    }
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override {
        // Initialization code when audio device is about to start
    }

    void audioDeviceStopped() override {
        // Cleanup code when audio device is stopped
        processingStop();
    }

private:

    std::atomic<bool> play;

    int totalNodesToProcess;

};
