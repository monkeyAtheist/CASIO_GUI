#ifndef GUI_H
#define GUI_H

#include <gint/display.h>
#include <gint/keyboard.h>

#include "../item/item.hpp"
#include "../cursor/cursor.hpp"
#include "../event/eventTable.hpp"

#include <string>
#include <functional>
#include <vector>
#include <map>

struct gui_page
{
    int id = 0;
    std::string name = "Main";
    bool enabled = true;
    int backgroundColor = C_WHITE;

    gui_page() = default;
    gui_page(
        int _id,
        std::string _name,
        bool _enabled = true,
        int _backgroundColor = C_WHITE)
        : id(_id),
          name(_name),
          enabled(_enabled),
          backgroundColor(_backgroundColor)
    {
    }
};

class GUI
{
public:
    GUI();
    ~GUI() = default;

    void checkHover();
    void checkClick();
    void checkItemEvents();
    void checkEvents();

    void draw()
    {
        cursor.setFocused(
            item.getKeyboardCapture() != nullptr
        );

        item.draw();
        cursor.draw();
    }
    bool runGUI();

    void showCursor() {cursor.show(); checkHover();}
    void hideCursor() {cursor.hide(); checkHover();}
    void setCursorVisible(bool visible) {cursor.setVisible(visible); checkHover();}
    bool isCursorVisible() const {return cursor.isVisible();}

    bool setFocusById(int id) {return item.setFocusById(id);}
    void clearFocus() {item.clearFocus();}
    bool focusNext() {return item.focusNext();}
    bool focusPrevious() {return item.focusPrevious();}
    int getFocusedItemId() const {return item.getFocusedItemId();}

    //******************************** Pages *********************************
    int createPage(const std::string& name, int backgroundColor = C_WHITE);
    bool setPage(int pageId);
    bool setPage(const std::string& name);
    int getCurrentPageId() const {return currentPageId;};
    unsigned int getPageCount() const {return pages.size();};

    gui_page* getPage(int pageId);
    const gui_page* getPage(int pageId) const;
    int getPageId(const std::string& name) const;
    bool setPageEnabled(int pageId, bool enabled);
    bool setPageBackgroundColor(int pageId, int color);
    int getCurrentPageBackgroundColor() const;

    // Convenience wrappers. Existing gui.item.addItem(...) remains valid.
    int addItem(::item* currentItem) {return item.addItem(currentItem);};
    int addItemToPage(int pageId, ::item* currentItem);
    int addGlobalItem(::item* currentItem);
    bool moveItemToPage(int itemId, int pageId);

    // Apply a theme to all existing items. By default, page backgrounds also
    // follow theme.background.
    void applyTheme(
        const item_theme& theme,
        bool updatePageBackgrounds = true
    );

    void setThemePreset(
        item_theme_preset preset,
        bool updatePageBackgrounds = true
    );

    const item_theme& getTheme() const
    {
        return current_item_theme();
    };

    //******************************** Screenshot *****************************
    void enableScreenshotCapture(
        int key = KEY_OPTN,
        const std::string& defaultName = "image"
    );

    void disableScreenshotCapture();
    bool isScreenshotCaptureEnabled() const {return screenshotEnabled;};

    void openScreenshotPrompt();

    void setScreenshotResultCallback(
        std::function<void(
            bool,
            const std::string&
        )> callback)
    {
        screenshotResultCallback = callback;
    };

    bool wasLastScreenshotSuccessful() const
    {
        return lastScreenshotSuccess;
    };

    const std::string& getLastScreenshotPath() const
    {
        return lastScreenshotPath;
    };

    liste_item item;
    listEventTable guiEvent;
    Cursor cursor{DWIDTH / 2, DHEIGHT / 2};
    key_event_t casioEvent;

    protected:
    private:
    bool checkScreenshotShortcut();
    void processPendingScreenshot();

    std::map<int, gui_page> pages;
    int currentPageId = 0;
    int nextPageId = 1;

    item_prompt_popup screenshotPrompt{
        "Screenshot",
        "Image name:",
        "image",
        24,
        nullptr,
        false,
        1000100
    };

    bool screenshotPopupRegistered = false;
    bool screenshotEnabled = false;
    int screenshotShortcutKey = KEY_OPTN;
    std::string screenshotDefaultName = "image";

    bool screenshotPending = false;
    std::string screenshotPendingName = "image";

    bool lastScreenshotSuccess = false;
    std::string lastScreenshotPath;

    std::function<void(
        bool,
        const std::string&
    )> screenshotResultCallback = nullptr;
};

/*

    // Exemple usage of the GUI class in main.cpp

    GUI gui;

    gui.cursor.setStyle(Cursor::cursorStyle::CROSS);
    gui.hideCursor();
    gui.showCursor();

*/

#endif // GUI_H
