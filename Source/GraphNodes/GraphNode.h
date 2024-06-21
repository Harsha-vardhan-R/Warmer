/*
  ==============================================================================

    GraphNode.h
    Created: 22 Feb 2024 4:51:24pm
    Author:  harshavardhan

  ==============================================================================
*/


#pragma once
#include <JuceHeader.h>

#include <memory>
#include "../ColourPalette.h"
#include "InputOutputTypesForSokets.h"
#include "Connection.h"
#include "MyParameterCtrls.h"
#include "../XML_Handling.h"


#define PI 3.141592653589793238462643f
#define PI_HALF 1.57079632679f
#define TAU 6.28318530718f

class EmptyLookAndFeel : public juce::LookAndFeel_V4 {
public:

    EmptyLookAndFeel() {
        setColour(juce::Slider::ColourIds::backgroundColourId, GraphSliderBackgroundID);
        setColour(juce::Slider::ColourIds::thumbColourId, GraphSliderThumbID);
        setColour(juce::Slider::ColourIds::trackColourId, GraphSliderOutlineID);
        setColour(juce::Slider::ColourIds::textBoxTextColourId, GraphSliderTextID);

        setColour(juce::ComboBox::ColourIds::backgroundColourId, MenuBackgroundID);
        setColour(juce::ComboBox::ColourIds::textColourId, textColourID);
        setColour(juce::ComboBox::ColourIds::arrowColourId, ComboBoxArrowColourID);

        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::lightgrey);

    }

    void drawComboBox(juce::Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY,
                                     int buttonW, int buttonH, juce::ComboBox& box) override {
        g.setColour(GraphSliderOutlineID);
        g.drawRoundedRectangle(box.getLocalBounds().toFloat() ,3.0f, 2.0);
        g.setColour(MenuMouseOverColourID);
        g.fillRoundedRectangle(box.getLocalBounds().reduced(2).toFloat(), 3.0f);

        juce::Path arrow;
        float arrowDimen = 5;
        float arrowX = width - 15; // X position of the arrow
        float arrowY = (height - arrowDimen) / 2; // Y position of the arrow

        if (box.isPopupActive()) {
            arrow.addTriangle(arrowX + (arrowDimen * 0.5f) + 1, arrowY,
                              arrowX, arrowY + arrowDimen,
                              arrowX + arrowDimen + 2, arrowY + arrowDimen);
        } else {
            arrow.addTriangle(arrowX, arrowY,
                              arrowX + arrowDimen + 2, arrowY,
                              arrowX + (arrowDimen * 0.5f) + 1, arrowY + arrowDimen);
        }

        g.setColour(ComboBoxArrowColourID);
        g.fillPath(arrow);
    }

    void drawPopupMenuItem (juce::Graphics& g,
                                           const juce::Rectangle<int>& area,
                                           bool isSeparator,
                                           bool isActive,
                                           bool isHighlighted,
                                           bool isTicked,
                                           bool hasSubMenu,
                                           const juce::String& text,
                                           const juce::String& shortcutKeyText,
                                           const juce::Drawable* icon,
                                           const juce::Colour* textColourToUse) override {

        juce::Rectangle<int> highLight(area.getX() + 3, area.getY(), area.getWidth() - 6 , area.getHeight());
        juce::Rectangle<int> aligner(area.getX() + 15, area.getY(), area.getWidth() , area.getHeight());

        auto width = area.getWidth();


        if (isSeparator) {
            g.setColour(SeparatorColourID);
            juce::Rectangle<int> sepLine(area.getX()+3, area.getCentreY(), area.getWidth()-6, 1);
            g.fillRect(sepLine);
            return;
        } else if (isHighlighted) {
            g.setColour(MenuMouseOverColourID);
            g.fillRoundedRectangle(highLight.toFloat(), 3.0f);
            g.setColour(SelectedTabTextColourID);
        } else {
            g.setColour(IdleTabTextColourID);
        }

        // Draw a side arrow if there is a submenu.
        if (hasSubMenu) {
            juce::Path arrow;
            arrow.addTriangle( width - 12,  6,
                               width - 6,  10,
                               width - 12,  14);
            g.setColour(ComboBoxArrowColourID);
            g.fillPath(arrow);
        }

        g.drawText(text, aligner, juce::Justification::centredLeft);

    }

    void drawPopupMenuBackground(juce::Graphics &g, int width, int height) override {
        g.setColour(MenuBackgroundID);
        g.fillAll();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EmptyLookAndFeel)
};

class menuStyleJuceNode : public juce::LookAndFeel_V3 {
public:
    menuStyleJuceNode() {
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::lightgrey);
    }
};

// Class that all the nodes inherit.
class GraphNode : public juce::Component,
                  public makeParseXML {
public :



    // the menu id of this particular node.
    // will be set after constructing a node in the add Node callback.
    int menuResultID = 0;

    void setMenuResultID(int a) {
        menuResultID = a;
    }

    int getMenuResultID() const {
        if (menuResultID == 0) {
            std::cout << "GetMenuResultId is called on " << name << " before assigning it" << "\n";
            return 0;
        }
        return menuResultID;
    }

    juce::String getAddressAsString() {
        auto pointer_val = reinterpret_cast<uint64_t>(this);
        return juce::String(pointer_val);
    }

    class menuStyleJuce : public juce::LookAndFeel_V3 {
    public:
        menuStyleJuce() {
            setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::lightgrey);
        }
    };

    juce::XmlElement * makeXML() override;
    // here we only set the parameter values and not any connections,
    // connections are set through XMLParseHelper.
    void parseXMLChildren(juce::XmlElement* x) override;
    // special function that can do special things(probably the most useless comment).
    // used for automating connections.
    // map_ contains the information about the unique ID and it's present address.
    void XMLParseHelper(juce::XmlElement* xmlEle, const std::unordered_map<juce::String, GraphNode*>& map_);
    // Some nodes need extra information to store
    // this method needs to be overridden to let that happen.
    virtual void mini_makeXML(juce::XmlElement*) {}
    // similarly this is called before the connections are made
    // and the parameters are set so any extra information that
    // is exclusively stored can be parsed and processed accordingly.
    virtual void mini_parseXMLChildren(juce::XmlElement* x) {}

    // Socket is a block inside the GraphNode.
    // contrary to the name you can actually have a parameterCtrl without the socket to control,
    // the behaviour will be controlled from the value.
    class Socket : public juce::Component,
                   public makeParseXML {
    public:

        menuStyleJuce menu_style;

        direction dir;

        // if this is empty then we do not draw a socket input for this socket.
        std::set<SocketDataType> TypesAccepted;

        juce::String name;

        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // xml related stuff.
        juce::XmlElement * makeXML() override {
            return parameterController.makeXML();
        }

        void parseXMLChildren(juce::XmlElement* x) override {
            parameterController.parseXMLChildren(x->getFirstChildElement());
        }

        // +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
        // API calls used from the GraphNode while processing.
        // set and get sets and gets the

        // gets the value from the parameter ctrl,
        // or if connected from the previous socket.
        // not to be called on output sockets.
        float getValue();
        juce::AudioBuffer<float>* getBufferPointer();
        juce::MidiBuffer* getMidiBuffer();

        // set value to all the connections this socket is connected to.
        void setFloatValue(float f);
        void setBufferPointer(juce::AudioBuffer<float>* bufferPointer);
        void setMidiBufferPointer(juce::MidiBuffer* midiMessagePointer);


        // isMust must be connected or else the queue will not be built.
        // for example an empty input for audio signal will stop the whole process.
        Socket( juce::String name,
                direction dir,
                bool isMust);

        // Be careful while using this,
        // The program will have undefined behaviour if there is no implementation for this type in the process method.
        void acceptType(SocketDataType type);

        // called after a successful connection , sets the variables and others.
        void connected(Socket* otherPointer, Connection* connection, bool flag = true);

        // check if this node can accept this type.
        bool accepts(SocketDataType type) ;

        // If the parameter control is set this will update.
        void update();

        // if the node is on the input side then, this will return
        // a pointer to the node it is from.
        // this should only be called to check on for input sockets.
        // for the output sockets we will get the prev nodes by calling it on the
        // next node.
        void* getPrevNode();

        // returns the point to draw the connection line from or to.
        juce::Point<int> getPivotPos();

        void paint (juce::Graphics& g) override;

        // Add a custom parameter control
        // #### MOST OF THESE FUNCTIONS ARE MEANT TO BE USED ONLY BY dir `IN` Sockets. ####
        //
        void addMenuParameterControl();
        //
        void addSliderParameterControl(float from, float to, float default_);
        //
        void addEnvParameterControl();
        //
        void addFilterDisplayControl();


        // if this is called you will have a mini_reset callback from a slider value
        // change.
        void setSliderCallbackWanted() { parameterController.setSilderWantedCallback(); }

        // If the parameter control is set to MenuListParameterCtrl, this is used to add new types of menu options.
        // else it is not going to change anything and there is no reason to use this method.
        void addMenuItem(juce::String name);

        void selectMenuItem(int index);

        // setting the type of output node,
        // this is the type that is checked against the set of input types that can be
        // accepted from an input node.
        void setOutputType(SocketDataType a);

        SocketDataType getOutputType();

        // like getOutputType but this returns the type of the connection presently,
        // meant to be called on an IN, if the socket can accept multiple types
        // you can use this to know what type of input is connected now,
        // would not make any sense to call this on an output node.
        SocketDataType getConnectionType();

        juce::PopupMenu menu;

        // relative to the edit page.
        juce::Point<float> getRelToGlobal(juce::Point<int> p);

        //deleting the socket connection.
        void mouseDown(const juce::MouseEvent& event) override;

        static void deleteSocketCallBack(int result, Socket* thisInstance);

        // called from the socket this is connected to.
        void deletedThisConnectionFrom(Socket* thatSocket);


        // using from two different methods.
        Connection* newConnection = nullptr;
        // adding a new connection.
        void mouseDrag(const juce::MouseEvent& event) override;

        // adding a new connection.
        void mouseUp(const juce::MouseEvent& event) override;

        bool isThisConnected() const { return isConnected; }

        void resized() override;

        bool isMust;

        // +++++++++++++++++++++++++++++++
        int repaintConnection();

        // internal API call to  delete the connections from a socket,
        // if called on an input socket delete only the connection,
        // for an output socket we delete all the connections from it.
        void deleteConnections();

        ~Socket() override;

        std::set<Socket*> to;

        bool hasParameterController() {
            return (parameterController.getType() != -1);
        }

        int getTotHeight() {
            if (isConnected && collapseOnConnection) return 20;
            else return 20 + parameterController.getHeight();
        }


        GraphNode* parentNodePointer = nullptr;

        // set by the parent while calling the update sockets method
        // so that we have a parent to call to when we have to trigger the parents mini-reset.
        void setParent(GraphNode* nodeParent) {
            parentNodePointer = nodeParent;
        }

        GraphNode* getParent() const {
            return parentNodePointer;
        }


        // This is the function that is going to get called from the
        // Parameter Ctrl if the method `triggerSocketsTrigger` is called in the listener method.
        // this call then sends the signal to the parent Graph node,
        // where we change callback or something similar based on the changed state.
        static void triggerGraphNodeMiniReset(Socket* thisInstance) {
            if (thisInstance->parentNodePointer) thisInstance->parentNodePointer->mini_reset();
        }


        // if this is called on a socket,
        // even if it is connected the parameter ctrl will not get collapsed.
        void setParameterCtrlNoCollapseOnConnection();

        bool collapseOnConnection = true;

        // returns pointer from the parameterCtrl class.
        void* getPointerToPresentParameterCtrlController() {
            return parameterController.getPresentParameterCtrlPointer();
        }

        void thisSocketsIndexIs(int a) {
            indexAtParent = a;
        }

        int getSocketsIndexInParent() {
            return indexAtParent;
        }

    private:

        int indexAtParent = -1;

        juce::Component* editPage;

        bool isConnected = false;
        Socket* from = nullptr;

        int numberOfSocketsConnectedTo = 0;


        class parameterCtrlLookAndFeel : public juce::LookAndFeel_V3 {
        public:

            parameterCtrlLookAndFeel() {
                setColour(juce::Slider::ColourIds::textBoxTextColourId, GraphSliderTextID);
                setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::grey);
                setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::white);
                setColour(juce::Slider::ColourIds::thumbColourId, GraphSliderThumbID);

                setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::lightgrey);
                setColour(juce::PopupMenu::ColourIds::backgroundColourId, juce::Colours::white);
                setColour(juce::PopupMenu::ColourIds::textColourId, textColourID);
                setColour(juce::PopupMenu::ColourIds::highlightedTextColourId, textSelectedColourID);
            }

            void drawPopupMenuItem (juce::Graphics& g,
                                    const juce::Rectangle<int>& area,
                                    bool isSeparator,
                                    bool isActive,
                                    bool isHighlighted,
                                    bool isTicked,
                                    bool hasSubMenu,
                                    const juce::String& text,
                                    const juce::String& shortcutKeyText,
                                    const juce::Drawable* icon,
                                    const juce::Colour* textColourToUse) override {

                juce::Rectangle<int> highLight(area.getX() + 3, area.getY(), area.getWidth() - 6 , area.getHeight());
                juce::Rectangle<int> aligner(area.getX(), area.getY(), area.getWidth() , area.getHeight());

                auto width = area.getWidth();

                if (isHighlighted) {
                    g.setColour(MenuMouseOverColourID);
                    g.fillRoundedRectangle(highLight.toFloat(), 3.0f);
                    g.setColour(SelectedTabTextColourID);
                } else {
                    g.setColour(IdleTabTextColourID);
                }

                g.drawText(text, aligner, juce::Justification::centred);

            }

        };

        // invisible container that contains other controls.
        class ParameterCtrl : public juce::Component,
                              private juce::Slider::Listener,
                              private juce::ComboBox::Listener,
                              public envParamCtrl::Listener,
                              public makeParseXML {
        public:

            inline juce::String arrayToDelimitedString(const std::vector<float>& vec) {
                juce::String new_;
                int flag = 0;

                for (auto i : vec) {
                    if (flag) new_ += juce::String(",");
                    new_ += juce::String(i);
                    flag = 1;
                }

                return new_;
            }

            static inline juce::String pointArrayToDelimitedString(const std::vector<envParamCtrl::Point>& vec) {
                juce::String new_;
                int flag = 0;

                for (auto i : vec) {
                    if (flag) new_ += juce::String(",");
                    new_ += juce::String("(");
                    new_ += juce::String(i.x);
                    new_ += juce::String(",");
                    new_ += juce::String(i.y);
                    new_ += juce::String(")");
                    flag = 1;
                }

                return new_;
            }

            static std::vector<envParamCtrl::Point> delimitedStringToPointArray(const juce::String& str) {
                std::vector<envParamCtrl::Point> vec;
                juce::String currentPoint;
                juce::String strCopy = str;

                for (char c : strCopy) {
                    if (c == '(') {
                        currentPoint = "(";
                    } else if (c == ')') {
                        int commaIndex = currentPoint.indexOf(0, ",");
                        float x = currentPoint.substring(1, commaIndex - 1).getFloatValue();
                        float y = currentPoint.substring(commaIndex + 1, currentPoint.length() - commaIndex - 2).getFloatValue();
                        vec.push_back({x , y});
                        currentPoint = "";
                    } else {
                        currentPoint += c;
                    }
                }

                return vec;
            }

            juce::XmlElement* makeXML() override {
                auto* parameterValue = new juce::XmlElement("Parameter");
                auto* parameterValueStore = new juce::XmlElement("valueStore");
                parameterValue->addChildElement(parameterValueStore);

                if (parameterType == -1) {
                    parameterValue->setAttribute("Type", "NULL");
                } else if (parameterType == 3) {
                    parameterValue->setAttribute("Type", "Envelope");
                    std::vector<envParamCtrl::Point> pointArray;
                    std::vector<float> controlStore;

                    if (envelopeCtrl) {
                        envelopeCtrl->copyData(pointArray, controlStore);
                    }

                    parameterValueStore->setAttribute("Controls", arrayToDelimitedString(controlStore));
                    parameterValueStore->setAttribute("Points", pointArrayToDelimitedString(pointArray));

                } else {
                    parameterValue->setAttribute("Type", "Value");
                    parameterValueStore->setAttribute("Value", getValue());
                }

                return parameterValue;
            }

            void parseXMLChildren(juce::XmlElement* x) override {

                std::cout << x->toString() << "\n";

                if (parameterType == -1) return;
                juce::XmlElement* child = x->getFirstChildElement();

                std::cout << child->toString() << "\n";

                if (parameterType == 1) {
                    // list.
                    int option = child->getIntAttribute("Value");
                    if (menuList) {
                        menuList->setSelectedId(option);
                    }
                } else if (parameterType == 2) {
                    double option = child->getDoubleAttribute("Value");
                    if (sliderFloat) {
                        sliderFloat->setValue(option);
                    }
                } else if (parameterType == 3) {
                    std::vector<float> controls;

                    for (juce::String& i : juce::StringArray::fromTokens(child->getStringAttribute("Controls"), ",", ""))
                        controls.push_back(i.juce::String::getFloatValue());

                    std::vector<envParamCtrl::Point> points = delimitedStringToPointArray(child->getStringAttribute("Points"));

                    if (envelopeCtrl) {
                        envelopeCtrl->setData(points, controls);
                    }
                } else {
                    std::cout << "What is this parameter?, I do not even know what it is?" << "\n";
                }

            }

            Socket* parentInstance = nullptr;

            explicit ParameterCtrl(Socket* parentInstance) {
                this->parentInstance = parentInstance;
                setVisible(false);
                valueAtomic.store(0.0);
            }

            ~ParameterCtrl() override = default;

            // getValue() is the abstraction that is called from process in the respective node to return the
            // selected item or a value.
            //
            // and because floating points can have precision errors better to compare with a greater than or less than
            // rather than equals to if you mean to return an integer.
            float getValue() {
                return valueAtomic.load();
            };

            int getType() const {
                return parameterType;
            }

            int getHeight() const {
                if (parameterType == -1) return 0;
                else if (parameterType == 3 || parameterType == 4) return 80;
                else return 30;
            }

            void createNewMenu() {
                if (parameterType != -1) {
                    std::cout << "Trying to rewrite the parameterCtrl type, not possible" << "\n";
                    return;
                }

                menuList = std::make_unique<juce::ComboBox>();
                menuList->addListener(this);

                menuList->setLookAndFeel(&styles);

                valueAtomic.store(0.0);
                parameterType = 1;
            }

            // call this if you need to call the mini reset on the graph node
            // that this parameterController is in.
            void triggerParent() const {
                triggerGraphNodeMiniReset(parentInstance);
            }

            // stores the selected value in a thread safe variable,
            // also sends a message to the GraphNode with the selected ID.
            void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override {
                valueAtomic.store(comboBoxThatHasChanged->getSelectedId());
                triggerParent();
            }

            void addItemToList(juce::String name_) {
                if (menuList) {
                    menuList->addItem(name_, index);
                    index++;
                }
            }

            void selectMenuItem(int val) {
                if (menuList) {
                    menuList->setSelectedId(val);
                }
            }

            void createTextEditor(float from_ , float to_ , float val) {
                if (parameterType != -1) {
                    std::cout << "Trying to rewrite the parameterCtrl type, not possible" << "\n";
                    return;
                }

                sliderFloat = std::make_unique<juce::Slider>();
                sliderFloat->setRange(from_, to_, 0.001);
                sliderFloat->setValue(val);
                sliderFloat->setSliderStyle(juce::Slider::LinearBar);
                sliderFloat->addListener(this);
                sliderFloat->setNumDecimalPlacesToDisplay(3);

                sliderFloat->setLookAndFeel(&styles);

                this->from = from_;
                this->to = to_;
                this->value = val;

                parameterType = 2;
            }

            void addEnvCtrlComponent() {
                envelopeCtrl = std::make_unique<envParamCtrl>(this);
                parameterType = 3;
            }

            void addFilterDisplayComponent() {
                filterDisplay = std::make_unique<filterTransferFunctionDisp>();
                parameterType = 4;
            }

            // called from the socket when ready to make it visible.
            void update() {
                // checking if it is not nullptr.
                if (menuList) {
                    menuList->setBounds(getLocalBounds().reduced(4));
                    addAndMakeVisible(menuList.get());
                    // do not ask me if you did not even add one option, dude.
                    menuList->setSelectedId(1, juce::sendNotificationSync);
                } else if (sliderFloat) {
                    sliderFloat->setBounds(getLocalBounds().reduced(4));
                    addAndMakeVisible(sliderFloat.get());
                    sliderFloat->setValue(value);
                } else if (envelopeCtrl) {
                    envelopeCtrl->setBounds(4, 4, getWidth()-8 , 72);
                    addAndMakeVisible(envelopeCtrl.get());
                } else if (filterDisplay) {
                    filterDisplay->setBounds(4, 4, getWidth()-8 , 72);
                    addAndMakeVisible(filterDisplay.get());
                }

            }

            // returns the pointer to envParameterCtrl.
            void* getPresentParameterCtrlPointer() {
                if (envelopeCtrl) return envelopeCtrl.get();
                else if (sliderFloat) return sliderFloat.get();
                else if (filterDisplay) return filterDisplay.get();
                else return menuList.get();
            }


            void setSilderWantedCallback() {
                wantCallbackFromSlider = true;
            }

            void sliderValueChanged(juce::Slider* slider) override {
                auto value_ = (float)sliderFloat->getValue();
                valueAtomic.store(value_);

                if (wantCallbackFromSlider) triggerParent();
            }

            void envParamCtrltListenerTriggered() override {
                triggerParent();
            }

            // lock and unlock while reading the values(presently not used anywhere).
            void lock() { mutex.lock(); }

            void unlock() { mutex.unlock(); }

            void paint(juce::Graphics& g) override {}

            void resized() override {}

            int parameterType = -1; // means nothing.
            // ^ 1 for menu and two for the text input.

        private:

            // set from the callbacks and read from get value.
            std::atomic<float> valueAtomic{};

            parameterCtrlLookAndFeel styles;

            // This is to lock this when others are reading or setting it.
            // not used anywhere in the code base now (but some parameter controls have mutex locks though)
            // because we process nodes one by one we do not set and get on the same socket at the same time.
            std::mutex mutex;


            // THE MENU
            // basically a list of values from which one can be selected.
            std::unique_ptr<juce::ComboBox> menuList = nullptr;
            int index = 1;


            // THE SLIDER : used to control many parameters.
            // we will be able to set the value directly, drag the mouse to change the value or use another node to control this.
            // when we are sliding or setting the value directly in the node, we do not record what the user is doing so
            // the resolution of changing will be of the buffer size, but connecting to an external node or a UI item will
            // give a smooth change in the controls - this is how it is designed to work, NOT a BUG.
            // value is always floating.
            std::unique_ptr<juce::Slider> sliderFloat = nullptr;
            float from{}, to{}, value{};
            bool wantCallbackFromSlider = false;


            // used to create the envelope.
            // used in Envelope.h
            std::unique_ptr<envParamCtrl> envelopeCtrl = nullptr;


            // display the transfer function in a Component(non-interactive)
            std::unique_ptr<filterTransferFunctionDisp> filterDisplay = nullptr;


            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterCtrl)
        };

        ParameterCtrl parameterController;

        juce::Point<int> lastMousePosition;

        // Only set for Output nodes.
        SocketDataType type = SocketDataType::NULLType;

        Connection* connectionPointer = nullptr;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Socket)
    };

    // Name of the node(shown at the top)
    juce::String name;

    menuStyleJuceNode menu_style;

    /*
     * Returns a vector of dependent nodes.
     */
    std::vector<GraphNode*> getDependencies();

    // returns a vector of values that contains all the nodes this is output to.
    std::set<GraphNode*> getDependents();

    // returns the Nodes that are dependent on the AudioBuffer that is written on by this node,
    // if none exist we are going to return an empty vector.
    std::set<GraphNode*> getAudioBufferDependencies();

    void doNotRecycleBuffer() { canBeRecycled = false; }
    bool getCanBeRecycled() const { return canBeRecycled; }

    // a quick kind of hack.
    // when a new connection is added the variable `needsRearrangement`
    // on the `from` node is read in the connectionAdded in the Instrument.cpp,
    // so for nodes like feedback we do not want to rearrange at the
    // output connection because we want to process the node before nodes it is connected to.
    // and after the node it is connected from.
    void noRearrangeAtConnection() { needsRearrangement = false; }
    bool needsRearrangementFromNode() const { return needsRearrangement; }

    juce::OwnedArray<Socket> InputSockets;


    // YOU CAN HAVE AT-MOST ONE SOCKET OF THE TYPE SocketDataType::AudioBufferFloat
    juce::OwnedArray<Socket> OutputSockets;


    // returns the address of the Input Socket at the provided index.
    // used when loading instruments and parsing the files.
    Socket* getInputSocketAtIndex(int index) {
        if (index < 0 || index >= InputSockets.size()) {
            std::cout << "Trying to ask for the address of a socket that does not exist(Index out of Bounds)" << "\n";
            return nullptr;
        }

        return InputSockets[index];
    }

    // The dimensions will be decided by the number of sockets.
    int UIWidth, UIHeight;

    // appears if you want to delete the node.
    juce::PopupMenu menu;


    // need to give the proportions for the node.
    GraphNode(juce::String name, int pos_x, int pos_y);

    juce::String getCurrentPosition() {
        juce::Point<int> p = getPosition();
        return juce::String(p.x)+juce::String(",")+juce::String(p.y);
    }

    // returns true if all the Input Sockets that are `isMust` are connected.
    virtual bool allGood();

    static void deleteNodeCallback(int result, GraphNode* graphNodeInstance);

    // for moving and delete menu.
    void mouseDown(const juce::MouseEvent& event) override;

    void mouseDrag(const juce::MouseEvent& event) override;

    void mouseUp(const juce::MouseEvent& event) override;

    // deletes all the connections that come to this node,
    // and are from this node
    void deleteAllConnectionsToAndFromNode();


    // Draws the common parts for all the nodes,
    // called before drawing the respective node related content.
    void paintBasic(juce::Graphics& g);

    // sets all the sockets in both input and output socket arrays to
    // addAndMakeVisible
    void makeAllSocketsVisible();


    void paint(juce::Graphics& g) override;

    void resized() override;

    ~GraphNode() override;

//    Copy and Implement for your own node.
//
//    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override {}
//    void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) override {}
//    void releaseResources() override {}
//    void reset() override {}

    double sampleRate;
    int estimatedSamplesPerBlock;

    // Pure virtual functions from the juce::AudioProcessor.
    void prepareToPlay(double sampleRate_, int estimatedSamplesPerBlock_) {
        this->sampleRate = sampleRate_;
        this->estimatedSamplesPerBlock = estimatedSamplesPerBlock_;
    }

    // called while destructing the node,
    // be very careful with shared data from the nodes.
    virtual void releaseResources() = 0;

    // this function is never used , we call the function called `processGraphNode`, to process the node.
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {}

    // called after the prepareToPlay is called,
    // use it if you want to configure more things after
    // setting the bufferSize and rate.
    //======
    // this should be used to set the call-back function pointer and the respective data that is,
    // needed for the respective callback function to work.
    //======
    // you basically manage the data while creating and removing a connection.
    // This function will be called every time a connection is added or removed from
    // any og the sockets connected to this
    //=====
    // the audio buffer that needs to get written will get the
    virtual void reset() = 0;

    // mini reset is called when a parameter is changed,
    // the parameterCtrl(if mentioned in the callback), will tell the
    // node processor to stop and call this function here.
    // no mutex locks or processing stops, do only atomic or non processGraphNode related stuff.
    virtual void mini_reset() {}


    // called from socket after deletion or creation of a new socket to the
    // input nodes.
    void repaintAllInputConnectionsToNode();


    juce::AudioBuffer<float>* bufferToWritePointer = nullptr;
    // set false if this node does not send out an audio buffer as an output.
    bool needAudioBuffer = true;

    // sets the buffer to write, done from the processing queue.
    void setToWriteAudioBuffer(juce::AudioBuffer<float>* b) { bufferToWritePointer = b; }

    // get the set bufferToWritePointer, if not set, returns nullptr
    juce::AudioBuffer<float>* getToWriteAudioBuffer() const { return bufferToWritePointer; }

    // the master function that is called from the AudioThread fo each audio buffer,
    // Make sure anything you access from this function is either thread synchronised,
    // or you are 100% sure that it is not going to be accessed from any other thread.
    //
    // The connection has all the stuff you just need, just ask it.
    virtual void processGraphNode() = 0;


    // +++++++++++++++++++++++

    bool canBeDeleted;


    // this not done by default because only a small
    // percentage of nodes need to be locked.
    // and mostly the use case is to change the callback pointer or something like that.
    std::mutex mutex;




private:

    EmptyLookAndFeel style;

    juce::Point<int> lastMouseDownPosition;

    bool canBeRecycled = true;
    // this is read by the connection Added in the Instrument
    // to see if we need to call the newConnection in the data structure.
    bool needsRearrangement = true;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};
