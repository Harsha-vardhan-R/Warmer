/*
  ==============================================================================

    MyDataStructures.h
    Created: 11 Mar 2024 9:27:39pm
    Author:  harshavardhan

  ==============================================================================
*/

#pragma once
#include "GraphNodes/Collection.h"

/*

    ***
    This is NOT designed to add new nodes continuously.

    A normal use would be like:
        -> `push` all the wanted nodes.
        -> use `sort`, to topo-sort the elements.
        -> process the nodes one by one until it reaches end repeat this process again.
   ***
*/

class PriorityQueue //: public juce::AudioProcessor
        {
public:

    PriorityQueue() {}

    ~PriorityQueue() { /* We are NOT deleting the nodes within the data structure with it */ }

    double sampleRate = 0.0, sampleSize = 0.0;

    void setBufferSizeAndRate(double sampleRate, int estimatedSamplesPerBlock) {
        this->sampleRate = sampleRate;
        this->sampleSize = estimatedSamplesPerBlock;
    }

    // Insert an element. O(1)
    void push(juce::AudioProcessor* node) {
        bunchOfNodes.insert(node);
        totalNodesToProcess++;
    }

    juce::AudioProcessor* MIDI_IN = nullptr;

    void setInputNode(InputMasterGraphNode* node) {
        MIDI_IN = (juce::AudioProcessor*)node;
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


    bool sort() {

        if (MIDI_IN == nullptr) return false;

        std::unordered_map<juce::AudioProcessor*, std::set<juce::AudioProcessor*>> AdjList;
        std::unordered_map<juce::AudioProcessor*, int> indegree;  // In-degree counter for each node
        std::queue<juce::AudioProcessor*> zeroDependencyQueue;    // Queue for nodes with zero dependencies

        if (!checkAllGood()) return false;

        // Prepare the Adjacent List and in-degree for each node
        for (auto i : bunchOfNodes) {
            AdjList[i] = ((GraphNode*)i)->getDependents();
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
        for (auto i : ((GraphNode*)MIDI_IN)->getDependents()) {
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

        for (auto i : sortedNodes) {
            std::cout << i->getName() << "\n";
        }


        if (sortedNodes.size() != bunchOfNodes.size()+1) {
            sortedNodes.clear();
            return false;
        }

        return true;  // No cycle
    }


    std::vector<juce::AudioProcessor*> getTopoSortedNodes() {
        return sortedNodes;
    }

    // Clears all the containers.
    void flush() {
        sortedNodes.clear();
        bunchOfNodes.clear();
    }

    // for debugging, data structure becomes useless after this, you need to build again.
    void debugDump() {
        std::cout << "Elements in Queue : " << "\n";
        for (auto i : sortedNodes) {
            std::cout << i << " ";
        }
    }

    std::vector<juce::AudioProcessor*> sortedNodes;
    std::set<juce::AudioProcessor*> bunchOfNodes;


//    virtual void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {}
//
//
//
//    // overriding function from juce::AudioProcessor //
//    virtual void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) {}
//    virtual void releaseResources() {}
//    virtual void reset() {}
//
//    // loading and saving of presets.
//    void getStateInformation(juce::MemoryBlock& destData) override {}
//    void setStateInformation(const void* data, int sizeInBytes) override {}
//    ///|=========================================|
//    // Virtual functions that have their definitions empty.
//    const juce::String getName() const override { return juce::String("The greater good"); }
//    double getTailLengthSeconds() const override { return 0.0; }
//    bool acceptsMidi() const override { return true; }
//    bool producesMidi() const override {return false; }
//    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
//    bool hasEditor() const override { return false; }
//    int getNumPrograms() override { return 1; }
//    int getCurrentProgram() override { return 0; }
//    void setCurrentProgram(int index) override {}
//    const juce::String getProgramName(int index) override { return juce::String("Warmer"); }
//    void changeProgramName(int index, const juce::String& newName) override {}

private:


    int totalNodesToProcess;

};