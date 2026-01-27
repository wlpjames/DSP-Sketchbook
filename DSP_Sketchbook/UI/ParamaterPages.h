/*
  ==============================================================================

    ParamaterPages.h
    Created: 8 May 2025 3:26:41pm
    Author:  William James

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "StyledSlider.h"

//==============================================================================
/*
*/
namespace sketchbook
{

class ExpandableListBox : public juce::Viewport
{
    public:
    class ExpandableListItem : public juce::Component
    {
        public:
        virtual int getFullHeight()=0;
    };
    
    class ListItemHolder : public juce::Component
    {
        public:
        class Header : public juce::Component
        {
            public:
            
            Header(juce::String titleText)
            {
                addAndMakeVisible(titleLabel);
                titleLabel.setJustificationType(juce::Justification::centredLeft);
                titleLabel.setText(titleText.replace("_", " "), juce::dontSendNotification);
                titleLabel.setFont(juce::FontOptions("Andale Mono", 14.f, juce::Font::plain));
                arrowIcon.reset(juce::Drawable::createFromImageData(DSP_SKETCHBOOK_BINARY::arrowdown_svg,
                                                                    DSP_SKETCHBOOK_BINARY::arrowdown_svgSize).release());
                animatorUpdater.addAnimator(arrowAnimator);
            }
            
            void resized() override
            {
                titleLabel.setBounds(getLocalBounds());
                arrowIcon->setBounds(getLocalBounds().removeFromRight(Height*1.5).removeFromLeft(Height * 0.7));
            }
            
            void paint(juce::Graphics& g) override
            {
                arrowIcon->setTransform(juce::AffineTransform::rotation(arrowAngleRadians, arrowIcon->getWidth() * 0.333, arrowIcon->getHeight() * 0.333));
                arrowIcon->drawWithin(g, arrowIcon->getBounds().toFloat(), juce::RectanglePlacement::centred, 1.0);
            }
            
            void startAnimation()
            {
                arrowAnimator.start();
            }
            
            static const int Height = 25;
            
            private:
            
            juce::Label titleLabel;
            std::unique_ptr <juce::Drawable> arrowIcon;
            
            //for dropdown animation
            float arrowAngleRadians = 0.0f;
            const float maxAngleRadians = juce::MathConstants<float>::pi;
            bool isOpen = false;
            
            juce::VBlankAnimatorUpdater animatorUpdater {this};
            juce::Animator arrowAnimator = juce::ValueAnimatorBuilder()
                .withEasing(juce::Easings::createLinear())
                .withDurationMs (150)
                .withValueChangedCallback ([this] (float value)
                                           {
                    arrowAngleRadians = (isOpen ? 1.f - value : value) * maxAngleRadians;
                    repaint();
                })
                .withOnCompleteCallback([this] ()
                                        {
                    isOpen = !isOpen;
                })
                .build();
        };
        
        ListItemHolder(ExpandableListItem* content, juce::String headerTitle)
        : m_header(headerTitle)
        , m_content(content)
        {
            jassert(content);
            addAndMakeVisible(m_header);
            addAndMakeVisible(m_content);
            m_header.addMouseListener(this, true);
            animatorUpdater.addAnimator(dropDownAnimator);
        }
        
        void resized() override
        {
            auto area = getLocalBounds().reduced(12, 0)
                .withTrimmedTop(currentVerticalPadding)
                .withTrimmedBottom(minVerticalPadding);
            
            m_header.setBounds(area.removeFromTop(ListItemHolder::Header::Height));
            if (m_content)
                m_content->setBounds(area);
        }
        
        int getCurrentHeight()
        {
            return Header::Height + currentContentHeight + currentVerticalPadding + minVerticalPadding;
        }
        
        void mouseUp(const juce::MouseEvent& event) override
        {
            dropDownAnimator.start();
            m_header.startAnimation();
        }
        
        void paint(juce::Graphics& g) override
        {
            g.setColour(juce::Colour::fromRGB(25, 25, 25));
            g.fillRoundedRectangle(getLocalBounds().reduced(6, 0)
                                   .withTrimmedTop(currentVerticalPadding)
                                   .toFloat(), 4);
        }
        
        std::function<void()> onSizeChange;
        
        private:
        
        bool isOpen = false;
        int currentContentHeight = 0;
        
        const float maxVerticalPadding = 10.0f;
        const float minVerticalPadding = 4.0f;
        float currentVerticalPadding = minVerticalPadding;
        
        ListItemHolder::Header m_header;
        ExpandableListItem* m_content = nullptr;
        
        //for dropdown animation
        juce::VBlankAnimatorUpdater animatorUpdater {this};
        juce::Animator dropDownAnimator = juce::ValueAnimatorBuilder()
            .withEasing(juce::Easings::createEaseInOutCubic())
            .withDurationMs (150)
            .withValueChangedCallback ([this] (float value)
                                       {
                float multiple = isOpen ? 1.0f - value : value;
                currentContentHeight = m_content->getFullHeight() * multiple;
                currentVerticalPadding = juce::jmap(multiple, 0.0f, 1.0f, minVerticalPadding, maxVerticalPadding);
                
                if (onSizeChange)
                    onSizeChange();
            })
            .withOnCompleteCallback([this] ()
                                    {
                isOpen = !isOpen;
            })
            .build();
    };
    
    class ListHolder : public juce::Component
    {
        public:
        
        ListHolder()
        {
            
        }
        
        ~ListHolder()
        {
            for (auto item : m_items)
            {
                if (item)
                {
                    delete item;
                    item=nullptr;
                }
            }
        }
        
        void resized() override
        {
            auto area = getLocalBounds();
            for (auto item : m_items)
                item->setBounds(area.removeFromTop(item->getCurrentHeight()));
        }
        
        
        void addItem(ExpandableListItem* item, juce::String headerText)
        {
            auto itemHolder = new ListItemHolder(item, headerText);
            m_items.add(itemHolder);
            addAndMakeVisible(itemHolder);
            itemHolder->onSizeChange = [this] ()
            {
                setSize(getWidth(), calcHeight());
            };
            setSize(getWidth(), calcHeight());
        }
        
        int calcHeight()
        {
            int h = 0;
            for (auto& item : m_items)
                h += item->getCurrentHeight();
            
            return h;
        }
        
        juce::Array<ListItemHolder*> m_items;
    };
    
    ExpandableListBox()
    {
        setViewedComponent(&listHolder);
        addAndMakeVisible(&listHolder);
        setScrollBarsShown(true, false);
        getVerticalScrollBar().setAutoHide(false);
    }
    
    void addItem(ExpandableListItem* item, juce::String headerText)
    {
        listHolder.addItem(item, headerText);
    }
    
    void resized() override
    {
        listHolder.setBounds(juce::Rectangle<int>(getScrollBarThickness(), 0, getWidth() - getScrollBarThickness(), listHolder.getHeight()));
    }
    
    ListHolder listHolder;
};

class ModuleGroupPage : public ExpandableListBox, public juce::ValueTree::Listener
{
    class EnablementRow : public juce::Component, public juce::ValueTree::Listener
    {
        public:
        EnablementRow()
        {
            addAndMakeVisible   (titleLabel);
            titleLabel.setJustificationType(juce::Justification::centredLeft);
            titleLabel.setFont(juce::FontOptions("Andale Mono", 14.f, juce::Font::plain));
            titleLabel.setText ("Enabled", juce::dontSendNotification);
            
            addAndMakeVisible   (onOffButton);
        }
        
        ~EnablementRow() override
        {
            if (data.isValid())
                data.removeListener (this);
        }
        
        void setData (juce::ValueTree _data)
        {
            //skip if new data invalid
            if (!_data.isValid())
                return;
            
            //remove listeners to old data
            if (data.isValid())
                data.removeListener (this);
            
            data = _data;
            data.addListener(this);
            
            
            onOffButton.setButtonText("on/off");
            onOffButton.setClickingTogglesState (true);
            onOffButton.setToggleState(data[Module::ParamIdents::ENABLED], juce::dontSendNotification);
            onOffButton.onStateChange = [this] ()
            {
                data.setProperty (Module::ParamIdents::ENABLED, onOffButton.getToggleState(), nullptr);
            };
            
            data.addListener (this);
        }
        
        juce::StringArray getModulationSourceNames()
        {
            auto sources = data.getRoot().getChildWithName(Module::ParamIdents::MODULATION_SOURCES);
            if (!sources.isValid()) return {};
            
            juce::StringArray output;
            for (auto sourceModule : sources)
                output.add(sourceModule[Module::ParamIdents::NAME]);
            
            return output;
        }
        
        void resized() override
        {
            auto area = getLocalBounds();
            int w = getWidth() / 4;
            titleLabel.setBounds(area.removeFromLeft (w));
            onOffButton.setBounds(area.removeFromRight(w).reduced(3));
        }
        
        static const int getRowHeight()
        {
            return 35;
        }
        
        private:
        
        void valueTreePropertyChanged(juce::ValueTree &tree, const juce::Identifier &property) override
        {
            if (property == Module::ParamIdents::ENABLED)
            {
                onOffButton.setToggleState(bool(tree[property]), juce::dontSendNotification);
            }
        }
        
        private:
        juce::Label titleLabel;
        juce::TextButton onOffButton;
        juce::ValueTree data;
    };
    
    class ParameterRow : public juce::Component, public juce::ValueTree::Listener
    {
        public:
        ParameterRow(Context& ctx)
        : slider(ctx)
        {
            addAndMakeVisible   (parameterTitleLabel);
            parameterTitleLabel.setJustificationType(juce::Justification::centredLeft);
            parameterTitleLabel.setFont(juce::FontOptions("Andale Mono", 14.f, juce::Font::plain));
            
            addChildComponent(slider);
            addChildComponent(integerStepper);
            addChildComponent(toggleButton);
            addChildComponent(comboOptions);
            addChildComponent(modulationSources);
        }
        
        ~ParameterRow() override
        {
            if (data.isValid())
                data.removeListener (this);
        }
        
        void setData (juce::ValueTree _data)
        {
            if (!_data.isValid())
                return;
            
            if (data.isValid())
                data.removeListener (this);
            
            data = _data;
            data.addListener (this);
            
            switch (getParamType(data))
            {
                case Module::parameterType::floatParam:
                {
                    slider.setRange(data[Module::ParamIdents::MIN], data[Module::ParamIdents::MAX]);
                    slider.setData(data);
                    slider.setValue(data[Module::ParamIdents::VALUE], juce::sendNotification);
                    slider.onValueChange = [this]
                    {
                        data.setPropertyExcludingListener(this, Module::ParamIdents::VALUE, slider.getValue(), nullptr);
                    };
                    break;
                }
                case Module::parameterType::intParam:
                {
                    integerStepper.setSliderStyle(juce::Slider::IncDecButtons);
                    integerStepper.setRange(data[Module::ParamIdents::MIN], data[Module::ParamIdents::MAX]);
                    integerStepper.setValue(data[Module::ParamIdents::VALUE], juce::sendNotification);
                    integerStepper.onValueChange = [this]
                    {
                        data.setPropertyExcludingListener(this, Module::ParamIdents::VALUE, integerStepper.getValue(), nullptr);
                    };
                    break;
                }
                case Module::parameterType::booleanParam:
                {
                    toggleButton.setToggleState(data[Module::ParamIdents::VALUE], juce::sendNotification);
                    toggleButton.setButtonText("on/off");
                    toggleButton.setClickingTogglesState (true);
                    toggleButton.onClick = [this]
                    {
                        data.setPropertyExcludingListener(this, Module::ParamIdents::VALUE, toggleButton.getToggleState(), nullptr);
                    };
                    break;
                }
                case Module::parameterType::choiceParam:
                {
                    comboOptions.clear();
                    juce::StringArray options = juce::StringArray::fromTokens(data[Module::ParamIdents::PARAMETER_OPTIONS].toString(), ";", "");
                    comboOptions.addItemList(options, 1);
                    comboOptions.setSelectedItemIndex(options.indexOf(data[Module::ParamIdents::VALUE].toString()), juce::sendNotification);
                    comboOptions.onChange = [this, options]
                    {
                        data.setPropertyExcludingListener(this, Module::ParamIdents::VALUE, options[comboOptions.getSelectedItemIndex()], nullptr);
                    };
                    break;
                }
                default: break;
            };
            
            setControlVisibilityForParamType();
            parameterTitleLabel.setText (data[Module::ParamIdents::PARAMETER_NAME], juce::dontSendNotification);
            
            modulationSources.clear();
            modulationSources.addItemList(getModulationSourceNames(), 1);
            modulationSources.setTextWhenNothingSelected("Add Mapping");
            modulationSources.setTextWhenNoChoicesAvailable("No Modulation Sources Available");
            modulationSources.onChange = [sp = SafePointer<ParameterRow>(this)] ()
            {
                int index = sp->modulationSources.getSelectedItemIndex();
                sp->data.addChild(Module::ModifiedParameter::defaultMappingTo(sp->getModulationSourceNames()[index]), -1, nullptr);
                sp->modulationSources.setSelectedId(0, juce::dontSendNotification);
            };
        }
        
        juce::StringArray getModulationSourceNames()
        {
            auto sources = data.getRoot().getChildWithName(Module::ParamIdents::MODULATION_SOURCES);
            if (!sources.isValid()) return {};
            
            juce::StringArray output;
            for (auto sourceModule : sources)
                output.add(sourceModule[Module::ParamIdents::NAME]);
            
            return output;
        }
        
        void paint (juce::Graphics& g) override
        {
            
        }
        
        void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
        {
            if (property != Module::ParamIdents::VALUE) return;
            
            switch (getParamType(data))
            {
                case Module::parameterType::floatParam:
                {
                    slider.setValue(data[Module::ParamIdents::VALUE], juce::dontSendNotification);
                    break;
                }
                case Module::parameterType::intParam:
                {
                    integerStepper.setValue(data[Module::ParamIdents::VALUE], juce::dontSendNotification);
                    break;
                }
                case Module::parameterType::booleanParam:
                {
                    toggleButton.setToggleState(data[Module::ParamIdents::VALUE], juce::dontSendNotification);
                    break;
                }
                case Module::parameterType::choiceParam:
                {
                    juce::StringArray options = juce::StringArray::fromTokens(data[Module::ParamIdents::PARAMETER_OPTIONS].toString(), ";", "");
                    comboOptions.setSelectedItemIndex(options.indexOf(data[Module::ParamIdents::VALUE].toString()), juce::dontSendNotification);
                    break;
                }
                default: break;
            };
        }
        
        void resized() override
        {
            auto area = getLocalBounds();
            int w = getWidth() / 4;
            parameterTitleLabel .setBounds (area.removeFromLeft (w).reduced(3));
            
            if (modulationSources.isVisible())
            {
                modulationSources   .setBounds(area.removeFromRight(w).reduced(3));
            }
            
            slider              .setBounds(juce::Rectangle<int>(area).removeFromRight(w).reduced(0, 3));
            integerStepper      .setBounds(juce::Rectangle<int>(area).removeFromRight(w).reduced(0, 3));
            toggleButton        .setBounds(juce::Rectangle<int>(area).removeFromRight(w).reduced(0, 3));
            comboOptions        .setBounds(juce::Rectangle<int>(area).removeFromRight(w).reduced(0, 3));
        }
        
        static const int getRowHeight()
        {
            return 35;
        }
        
        private:
        
        Module::parameterType getParamType(juce::ValueTree parameterData)
        {
            if (parameterData.getType() == Module::ParamIdents::PARAMETER_FLOAT)
            {
                return Module::parameterType::floatParam;
            }
            else if (parameterData.getType() == Module::ParamIdents::PARAMETER_INTEGER)
            {
                return Module::parameterType::intParam;
            }
            else if (parameterData.getType() == Module::ParamIdents::PARAMETER_BOOL)
            {
                return Module::parameterType::booleanParam;
            }
            else if (parameterData.getType() == Module::ParamIdents::PARAMETER_CHOICE)
            {
                return Module::parameterType::choiceParam;
            }
            
            //default to floating point (but throw assert here anyway)
            jassertfalse;
            return Module::parameterType::floatParam;
        }
        
        void setControlVisibilityForParamType()
        {
            auto type = getParamType(data);
            modulationSources.setVisible(type == Module::parameterType::floatParam);
            slider           .setVisible(type == Module::parameterType::floatParam);
            integerStepper   .setVisible(type == Module::parameterType::intParam);
            toggleButton     .setVisible(type == Module::parameterType::booleanParam);
            comboOptions     .setVisible(type == Module::parameterType::choiceParam);
        }
        
        juce::Label parameterTitleLabel;
        
        StyledLinearSlider  slider;
        juce::Slider        integerStepper;
        juce::TextButton          toggleButton;
        juce::ComboBox            comboOptions;
        
        juce::ComboBox modulationSources;
        juce::ValueTree data;
    };
    
    class ExpandableModule : public ExpandableListItem
    {
        public:
        ExpandableModule(Context& _ctx)
        : ctx(_ctx)
        {
            
        }
        
        ~ExpandableModule() override
        {
            for (auto row : rows)
            {
                if (row)
                {
                    delete row;
                    row = nullptr;
                }
            }
        }
        
        void resized() override
        {
            auto area = getLocalBounds();
            for (auto row : rows)
            {
                row->setBounds(area.removeFromTop(35));
            }
        }
        
        int getFullHeight() override
        {
            return rows.size() * ParameterRow::getRowHeight();
        }
        
        void setData(juce::ValueTree newData)
        {
            data = newData;
            
            //create a row for module enablement
            if (data.hasProperty(Module::ParamIdents::ENABLED))
            {
                auto enablementRow = new EnablementRow();
                enablementRow->setData(data);
                addAndMakeVisible(enablementRow);
                rows.add(enablementRow);
            }
            
            for (auto row : data.getChildWithName(Module::ParamIdents::PARAMETERS))
            {
                auto paramRow = new ParameterRow(ctx);
                paramRow->setData(row);
                addAndMakeVisible(paramRow);
                rows.add(paramRow);
            }
            
            setSize(getWidth(), ParameterRow::getRowHeight() * rows.size());
        }
        
        private:
        
        juce::ValueTree getDataForRow (int rowNumber)
        {
            return data.getChildWithName(Module::ParamIdents::PARAMETERS).getChild(rowNumber);
        }
        
        juce::Array<juce::Component*> rows;
        juce::ValueTree data;
        sketchbook::Context& ctx;
    };
    
    public:
    
    ModuleGroupPage(Context& _ctx)
    : ctx(_ctx)
    {}
    
    void setData(juce::ValueTree newData)
    {
        data = newData;
        data.addListener(this);
        updateContent();
    }
    
    void updateContent()
    {
        auto tree = getModulesData(data);
        for (auto sourceData : tree)
        {
            auto panel = new ExpandableModule(ctx);
            panel->setData(sourceData);
            addItem(panel, sourceData[Module::ParamIdents::NAME]);
        }
        resized();
    }
    
    virtual juce::ValueTree getModulesData(juce::ValueTree fullData)=0;
    
    private:
    
    juce::ValueTree data;
    sketchbook::Context& ctx;
};

class Pages  : public juce::Component
{
    class ParametersPage : public ModuleGroupPage
    {
        public:
        
        ParametersPage(Context& _ctx)
        : ModuleGroupPage(_ctx)
        {}
        
        juce::ValueTree getModulesData(juce::ValueTree fullData)
        {
            return fullData.getChildWithName(Module::ParamIdents::MODULES);
        }
    };
    
    
    class ModulationSourcesPage : public ModuleGroupPage
    {
    public:
        
        ModulationSourcesPage(Context& _ctx)
        : ModuleGroupPage(_ctx)
        {}
        
        juce::ValueTree getModulesData(juce::ValueTree fullData) override
        {
            return fullData.getRoot().getChildWithName(Module::ParamIdents::MODULATION_SOURCES);
        }
    };
    
    class FXPage : public ModuleGroupPage
    {
    public:
        
        FXPage(Context& _ctx)
        : ModuleGroupPage(_ctx)
        {}
        
        juce::ValueTree getModulesData(juce::ValueTree fullData) override
        {
            return fullData.getRoot().getChildWithName(Module::ParamIdents::EFFECT_FILTERS);
        }
    };
    
    class ModulationsPage : public juce::ListBox, public juce::ListBoxModel, public juce::ValueTree::Listener
    {
        class ModulationRow : public juce::Component, public juce::ValueTree::Listener
        {
            public:
            ModulationRow(Context& ctx)
            : amount(ctx)
            {
                addAndMakeVisible (reverse);
                reverse.setButtonText ("Reverse");
                reverse.setClickingTogglesState (true);
                reverse.onClick = [this] ()
                {
                    data.setProperty (Module::ParamIdents::MOD_REVERSED, reverse.getToggleState(), nullptr);
                };
                
                addAndMakeVisible (biPolar);
                biPolar.setButtonText ("BiPolar");
                biPolar.setClickingTogglesState (true);
                biPolar.onClick = [this] ()
                {
                    data.setProperty (Module::ParamIdents::MOD_CENTRED, biPolar.getToggleState(), nullptr);
                };
                
                addAndMakeVisible (remove);
                
                addAndMakeVisible (amount);
                amount.setRange (-1.0f, 1.0f);
                amount.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
                amount.onValueChange = [this] ()
                {
                    data.setProperty(Module::ParamIdents::MOD_AMOUNT, amount.getValue(), nullptr);
                };
                
                addAndMakeVisible(sourceLabel);
                
                addAndMakeVisible (remove);
                remove.setButtonText ("X");
                remove.onClick = [this] ()
                {
                    if (data.isValid())
                    {
                        auto parent = data.getParent();
                        
                        if (parent.isValid())
                            parent.removeChild(data, nullptr);
                    }
                };
            }
            
            ~ModulationRow() override
            {
                
            }
            
            void setData(juce::ValueTree _data)
            {
                if (data.isValid())
                    data.removeListener (this);
                
                data = _data;
                data.addListener(this);
                
                juce::String source        = data[Module::ParamIdents::MODULATION_SOURCE].toString();
                
                //TODO: traversing by get parents will be prone to brake if structure changes
                juce::String destModule = data.getParent().getParent().getParent()[Module::ParamIdents::NAME].toString();
                juce::String destParam  = data.getParent().getParent()[Module::ParamIdents::PARAMETER_NAME].toString();
                
                sourceLabel.setText(source + " -> " + destModule + ": " + destParam, juce::dontSendNotification);
                amount.setValue (data[Module::ParamIdents::MOD_AMOUNT], juce::dontSendNotification);
                reverse.setToggleState (data[Module::ParamIdents::MOD_REVERSED], juce::dontSendNotification);
                biPolar.setToggleState (data[Module::ParamIdents::MOD_CENTRED], juce::dontSendNotification);
            }
            
            void paint (juce::Graphics& g) override
            {
                
            }
            
            void resized() override
            {
                auto area = getLocalBounds().reduced(0, 2);
                
                sourceLabel.setBounds(area.removeFromLeft(area.getWidth() * 0.45));
                amount.setBounds(area.removeFromLeft(area.getHeight()).reduced(1.5));
                
                int width = area.getWidth() / 3;
                area.reduce(0, 3.5);
                remove .setBounds(area.removeFromRight(width).reduced(3.5));
                reverse.setBounds(area.removeFromRight(width).reduced(3.5));
                biPolar.setBounds(area.removeFromRight(width).reduced(3.5));
            }
            
            private:
            
            StyledSlider    amount;
            juce::TextButton      reverse;
            juce::TextButton      biPolar;
            juce::TextButton      remove;
            juce::ValueTree       data;
            juce::Label           sourceLabel;
            
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulationRow)
        };
        
        public:
        ModulationsPage(Context& _ctx)
        : ctx(_ctx)
        {
            setModel(this);
            setColour(juce::ListBox::ColourIds::backgroundColourId, juce::Colours::white.withAlpha(0.f));
            setRowHeight(44);
        }
        
        ~ModulationsPage() override
        {
            
        }
        
        juce::Array<juce::ValueTree> recursiveSearch(juce::Identifier name, juce::ValueTree tree)
        {
            juce::Array<juce::ValueTree> output;
            for (auto child : tree)
            {
                if (child.hasType(name))
                    output.add(child);
                else
                    output.addArray(recursiveSearch(name, child));
            }
            return output;
        }
        
        void setData(juce::ValueTree newData)
        {
            data = newData;
            data.addListener(this);
            modulationDataList = recursiveSearch(Module::ParamIdents::MODULATION, data);
            updateContent();
        }
        
        void valueTreeChildAdded(juce::ValueTree &parentTree, juce::ValueTree &childWhichHasBeenAdded) override
        {
            if (childWhichHasBeenAdded.hasType(Module::ParamIdents::MODULATION))
            {
                modulationDataList = recursiveSearch(Module::ParamIdents::MODULATION, data);
                updateContent();
            }
        }
        
        void valueTreeChildRemoved(juce::ValueTree &parentTree, juce::ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override
        {
            if (childWhichHasBeenRemoved.hasType(Module::ParamIdents::MODULATION))
            {
                //rebuild the list instead of searching
                modulationDataList = recursiveSearch(Module::ParamIdents::MODULATION, data);
                updateContent();
            }
        }
        
        int getNumRows() override
        {
            return modulationDataList.size();
        }
        
        juce::Component* refreshComponentForRow (int rowNumber, bool isRowSelected, juce::Component *existingComponentToUpdate) override
        {
            auto data = getDataForRow(rowNumber);
            
            if (!data.isValid())
            {
                return nullptr;
            }
            
            if (existingComponentToUpdate != nullptr)
            {
                if (auto* row = dynamic_cast<ModulationRow*> (existingComponentToUpdate))
                {
                    row->setData (data);
                    return row;
                }
            }
            else
            {
                auto* row = new ModulationRow(ctx);
                row->setData(data);
                addAndMakeVisible (row);
                return row;
            }
            
            return nullptr;
        }
        
        void paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected) override
        {
            return;
        }
        
        juce::ValueTree getDataForRow(int rowNumber)
        {
            if (modulationDataList.size() > rowNumber)
                return modulationDataList.getReference(rowNumber);
            
            //Trying to access a row that doesn't exist
            return juce::ValueTree();
        }
        
        private:
        
        Context& ctx;
        juce::ValueTree data;
        juce::Array<juce::ValueTree> modulationDataList;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulationsPage)
    };
    
    public:
    
    Pages(Context& _ctx)
    : parametersPage(_ctx)
    , modulationSourcesPage(_ctx)
    , modulationsPage(_ctx)
    , fxPage(_ctx)
    , ctx(_ctx)
    {
        pageList.add(&parametersPage);
        pageList.add(&modulationsPage);
        pageList.add(&modulationSourcesPage);
        pageList.add(&fxPage);
        
        addChildComponent(parametersPage);
        addChildComponent(modulationsPage);
        addChildComponent(modulationSourcesPage);
        addChildComponent(fxPage);
        
        setColour(juce::TabbedComponent::ColourIds::backgroundColourId, juce::Colours::white.withAlpha(0.f));
        
        //show first page on start
        showPage(0);
    }
    
    ~Pages() override
    {
        
    }
    
    void setData(juce::ValueTree newData)
    {
        parametersPage.setData(newData);
        modulationSourcesPage.setData(newData);
        modulationsPage.setData(newData);
        fxPage.setData(newData);
    }
    
    void resized() override
    {
        for (auto p : pageList)
        {
            p->setBounds(getLocalBounds());
        }
    }
    
    void showPage(int pageIndex)
    {
        if (pageIndex < pageList.size() && pageIndex >= 0)
        {
            pageList[selectedPage]->setVisible(false);
            pageList[pageIndex]->setVisible(true);
            selectedPage = pageIndex;
        }
        
        resized();
    }
    
    private:
    
    ParametersPage parametersPage;
    ModulationSourcesPage modulationSourcesPage;
    ModulationsPage modulationsPage;
    FXPage fxPage;
    juce::Array<juce::Component*> pageList;
    int selectedPage = 0;
    Context& ctx;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pages)
};

} // end namespace sketchbook
