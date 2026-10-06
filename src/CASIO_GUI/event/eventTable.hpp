#ifndef GUI_EVENT_TABLE_HPP
#define GUI_EVENT_TABLE_HPP

#include <gint/display.h>
#include <gint/keyboard.h>

#include "../item/item.hpp"
#include "../cursor/cursor.hpp"

#include <string>
#include <functional>
#include <map>

// Unique key used by listEventTable.
// An event is identified by its type + its key.
// Example: KEY_DOWN + KEY_EXE is unique.
struct eventTableKey
{
    itemEvent::eventType type;
    int keyEvent;

    eventTableKey(
        itemEvent::eventType _type = itemEvent::eventType::KEY_DOWN,
        int _keyEvent = KEY_EXE)
        : type(_type), keyEvent(_keyEvent)
    {
    }

    bool operator<(const eventTableKey& other) const
    {
        if(type != other.type)
            return static_cast<int>(type) < static_cast<int>(other.type);

        return keyEvent < other.keyEvent;
    }

    bool operator==(const eventTableKey& other) const
    {
        return type == other.type && keyEvent == other.keyEvent;
    }
};

class eventTable
{
    public :
    eventTable() = default;
    eventTable(itemEvent::eventType _type, int _keyEvent, std::function<void()> _callback)
        : type(_type), keyEvent(_keyEvent), callback(_callback) {}
    ~eventTable() = default;

    // Getters
    void callCallback() {if(callback) callback();}
    int getKeyEvent() const {return keyEvent;}
    itemEvent::eventType getType() const {return type;}
    eventTableKey getEventKey() const {return eventTableKey{type, keyEvent};}

    // Setters
    void setType(itemEvent::eventType _type) {type = _type;}
    void setKeyEvent(int _keyEvent) {keyEvent = _keyEvent;}
    void setCallback(std::function<void()> _callback) {callback = _callback;}

    private:
    itemEvent::eventType type = itemEvent::eventType::KEY_DOWN;
    int keyEvent = KEY_EXE;
    std::function<void()> callback = nullptr;

    protected:
};

class listEventTable
{
    public :
    // Special key used for an event which concerns every keyboard key.
    // Example: a general KEY_DOWN event, independently of which key was pressed.
    static constexpr int EVENT_ANY_KEY = -1;

    listEventTable() = default;
    ~listEventTable() = default;

    int getEventCount() const {return listEvent.size();}

    // Add a specific event.
    // Returns false if the exact same event is already installed.
    bool addEvent(itemEvent::eventType _type, int _keyEvent, std::function<void()> _callback)
    {
        eventTableKey key{_type, _keyEvent};

        if(listEvent.find(key) != listEvent.end())
            return false;

        listEvent.emplace(key, eventTable{_type, _keyEvent, _callback});
        return true;
    }

    // Register the same callback for several event types at once.
    //
    // Example:
    //   addEvent(KEY_DOWN | KEY_PRESS, KEY_LEFT, callback);
    //
    // Internally the mask is expanded into ordinary single-type events. This
    // keeps dispatch fast and preserves the existing eventTableKey semantics.
    // Registration is atomic: if one requested event already exists, nothing
    // is added and the function returns false.
    bool addEvent(
        itemEvent::eventMask _types,
        int _keyEvent,
        std::function<void()> _callback)
    {
        if(_types.empty())
            return false;

        constexpr itemEvent::eventType supportedTypes[] =
        {
            itemEvent::eventType::HOVER,
            itemEvent::eventType::KEY_UP,
            itemEvent::eventType::KEY_DOWN,
            itemEvent::eventType::KEY_PRESS,
            itemEvent::eventType::DRAG
        };

        // Validate first so a failed registration never leaves a half-added
        // group of events.
        for(itemEvent::eventType type : supportedTypes)
        {
            if(_types.contains(type) && hasEvent(type, _keyEvent))
                return false;
        }

        bool added = false;

        for(itemEvent::eventType type : supportedTypes)
        {
            if(!_types.contains(type))
                continue;

            // The previous pass guarantees that this cannot fail because of a
            // duplicate key. std::function is intentionally copied so every
            // event owns a valid callback.
            listEvent.emplace(
                eventTableKey{type, _keyEvent},
                eventTable{type, _keyEvent, _callback}
            );
            added = true;
        }

        return added;
    }

    // Same multi-type registration for any keyboard key.
    bool addEvent(
        itemEvent::eventMask _types,
        std::function<void()> _callback)
    {
        return addEvent(_types, EVENT_ANY_KEY, _callback);
    }

    // Add a general event for an event type.
    // Example: KEY_DOWN for any key.
    bool addEvent(itemEvent::eventType _type, std::function<void()> _callback)
    {
        return addEvent(_type, EVENT_ANY_KEY, _callback);
    }

    // Remove by unique event key.
    bool removeEvent(itemEvent::eventType _type, int _keyEvent)
    {
        return listEvent.erase(eventTableKey{_type, _keyEvent}) > 0;
    }

    // Remove a general event.
    bool removeEvent(itemEvent::eventType _type)
    {
        return removeEvent(_type, EVENT_ANY_KEY);
    }

    // Compatibility with the previous index-based API.
    void removeEvent(unsigned int idx)
    {
        auto it = getIterator(idx);

        if(it != listEvent.end())
            listEvent.erase(it);
    }

    void clearEvents()
    {
        listEvent.clear();
    }

    // Call every installed event.
    void updateEvents()
    {
        for(auto& event : listEvent)
            event.second.callCallback();
    }

    // Call one specific event.
    bool updateEvent(itemEvent::eventType _type, int _keyEvent)
    {
        eventTable* event = getEvent(_type, _keyEvent);

        if(event == nullptr)
            return false;

        event->callCallback();
        return true;
    }

    // Call one general event.
    bool updateEvent(itemEvent::eventType _type)
    {
        return updateEvent(_type, EVENT_ANY_KEY);
    }

    // Compatibility with the previous index-based API.
    void updateEvent(unsigned int idx)
    {
        eventTable* event = getEvent(idx);

        if(event != nullptr)
            event->callCallback();
    }

    // Getters for individual events

    eventTable* getEvent(itemEvent::eventType _type, int _keyEvent)
    {
        auto it = listEvent.find(eventTableKey{_type, _keyEvent});

        if(it == listEvent.end())
            return nullptr;

        return &it->second;
    }

    const eventTable* getEvent(itemEvent::eventType _type, int _keyEvent) const
    {
        auto it = listEvent.find(eventTableKey{_type, _keyEvent});

        if(it == listEvent.end())
            return nullptr;

        return &it->second;
    }

    // Get a general event.
    eventTable* getEvent(itemEvent::eventType _type)
    {
        return getEvent(_type, EVENT_ANY_KEY);
    }

    const eventTable* getEvent(itemEvent::eventType _type) const
    {
        return getEvent(_type, EVENT_ANY_KEY);
    }

    // Compatibility with the previous index-based API.
    eventTable* getEvent(unsigned int idx)
    {
        auto it = getIterator(idx);

        if(it == listEvent.end())
            return nullptr;

        return &it->second;
    }

    eventTableKey getEventKey(unsigned int idx)
    {
        auto it = getIterator(idx);

        if(it == listEvent.end())
            return eventTableKey{};

        return it->first;
    }

    bool hasEvent(itemEvent::eventType _type, int _keyEvent) const
    {
        return listEvent.find(eventTableKey{_type, _keyEvent}) != listEvent.end();
    }

    bool hasEvent(itemEvent::eventType _type) const
    {
        return hasEvent(_type, EVENT_ANY_KEY);
    }

    // Replace an event.
    // The replacement is refused if its new unique key already exists.
    bool setEvent(
        itemEvent::eventType _oldType,
        int _oldKeyEvent,
        itemEvent::eventType _newType,
        int _newKeyEvent,
        std::function<void()> _callback)
    {
        eventTableKey oldKey{_oldType, _oldKeyEvent};
        eventTableKey newKey{_newType, _newKeyEvent};

        auto it = listEvent.find(oldKey);

        if(it == listEvent.end())
            return false;

        if(!(oldKey == newKey) && listEvent.find(newKey) != listEvent.end())
            return false;

        if(oldKey == newKey)
        {
            it->second.setCallback(_callback);
            return true;
        }

        listEvent.erase(it);
        listEvent.emplace(
            newKey,
            eventTable{_newType, _newKeyEvent, _callback}
        );

        return true;
    }

    // Compatibility with the previous index-based API.
    bool setEvent(
        unsigned int idx,
        itemEvent::eventType _type,
        int _keyEvent,
        std::function<void()> _callback)
    {
        auto it = getIterator(idx);

        if(it == listEvent.end())
            return false;

        eventTableKey oldKey = it->first;

        return setEvent(
            oldKey.type,
            oldKey.keyEvent,
            _type,
            _keyEvent,
            _callback
        );
    }

    bool setEventType(
        itemEvent::eventType _oldType,
        int _keyEvent,
        itemEvent::eventType _newType)
    {
        eventTableKey oldKey{_oldType, _keyEvent};
        eventTableKey newKey{_newType, _keyEvent};

        auto it = listEvent.find(oldKey);

        if(it == listEvent.end())
            return false;

        if(!(oldKey == newKey) && listEvent.find(newKey) != listEvent.end())
            return false;

        eventTable event = it->second;
        event.setType(_newType);

        listEvent.erase(it);
        listEvent.emplace(newKey, event);

        return true;
    }

    bool setEventType(unsigned int idx, itemEvent::eventType _type)
    {
        auto it = getIterator(idx);

        if(it == listEvent.end())
            return false;

        eventTableKey oldKey = it->first;
        return setEventType(oldKey.type, oldKey.keyEvent, _type);
    }

    bool setEventKey(
        itemEvent::eventType _type,
        int _oldKeyEvent,
        int _newKeyEvent)
    {
        eventTableKey oldKey{_type, _oldKeyEvent};
        eventTableKey newKey{_type, _newKeyEvent};

        auto it = listEvent.find(oldKey);

        if(it == listEvent.end())
            return false;

        if(!(oldKey == newKey) && listEvent.find(newKey) != listEvent.end())
            return false;

        eventTable event = it->second;
        event.setKeyEvent(_newKeyEvent);

        listEvent.erase(it);
        listEvent.emplace(newKey, event);

        return true;
    }

    bool setEventKey(unsigned int idx, int _keyEvent)
    {
        auto it = getIterator(idx);

        if(it == listEvent.end())
            return false;

        eventTableKey oldKey = it->first;
        return setEventKey(oldKey.type, oldKey.keyEvent, _keyEvent);
    }

    bool setEventCallback(
        itemEvent::eventType _type,
        int _keyEvent,
        std::function<void()> _callback)
    {
        eventTable* event = getEvent(_type, _keyEvent);

        if(event == nullptr)
            return false;

        event->setCallback(_callback);
        return true;
    }

    bool setEventCallback(unsigned int idx, std::function<void()> _callback)
    {
        eventTable* event = getEvent(idx);

        if(event == nullptr)
            return false;

        event->setCallback(_callback);
        return true;
    }

    // Trigger one specific event and, if installed, the general event
    // associated with the same event type.
    bool triggerEvent(itemEvent::eventType _type, int _keyEvent)
    {
        bool triggered = false;

        if(updateEvent(_type, _keyEvent))
            triggered = true;

        if(_keyEvent != EVENT_ANY_KEY && updateEvent(_type))
            triggered = true;

        return triggered;
    }

    private:
    std::map<eventTableKey, eventTable> listEvent;

    std::map<eventTableKey, eventTable>::iterator getIterator(unsigned int idx)
    {
        if(idx >= listEvent.size())
            return listEvent.end();

        auto it = listEvent.begin();

        for(unsigned int i = 0; i < idx; ++i)
            ++it;

        return it;
    }

    protected:
};

#endif // GUI_EVENT_TABLE_HPP
