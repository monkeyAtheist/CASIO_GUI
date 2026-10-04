#include "gui.hpp"
#include <gint/timer.h>
#include <gint/rtc.h>

#include "../screenshot/screenshot.hpp"


GUI::GUI()
{
    pages.emplace(0, gui_page{0, "Main", true, current_item_theme().background.getRGB()});

    cursor.setNormalColor(
        current_item_theme().text.getRGB()
    );

    cursor.setFocusColor(
        current_item_theme().warning.getRGB()
    );

    cursor.setFocusThickness(3);
    cursor.setFocusRadiusBonus(2);

    currentPageId = 0;
    item.setActivePageId(currentPageId);
}

void GUI::enableScreenshotCapture(
    int key,
    const std::string& defaultName)
{
    screenshotShortcutKey = key;

    screenshotDefaultName =
        defaultName.empty()
            ? "image"
            : defaultName;

    screenshotPrompt.setTitle(
        "Screenshot");

    screenshotPrompt.setMessage(
        "Image name (.bmp):");

    screenshotPrompt.setMaxLength(24);

    screenshotPrompt.setOnSubmit(
        [this](const std::string& value)
        {
            screenshotPendingName =
                value.empty()
                    ? screenshotDefaultName
                    : value;

            screenshotPending = true;
        });

    if(!screenshotPopupRegistered)
    {
        screenshotPrompt.setGlobalPage();

        item.addItem(
            &screenshotPrompt);

        screenshotPopupRegistered = true;
    }

    screenshotEnabled = true;
}


void GUI::disableScreenshotCapture()
{
    screenshotEnabled = false;
    screenshotPending = false;

    if(screenshotPrompt.isOpen())
        screenshotPrompt.close();
}


void GUI::openScreenshotPrompt()
{
    if(!screenshotEnabled)
        return;

    screenshotPrompt.setValue(
        screenshotDefaultName);

    screenshotPrompt.resetInputModifiers();
    screenshotPrompt.setAlphaLock(true);
    screenshotPrompt.open();

    checkHover();
}


bool GUI::checkScreenshotShortcut()
{
    if(
        !screenshotEnabled ||
        casioEvent.type != KEYEV_DOWN ||
        casioEvent.key != screenshotShortcutKey)
    {
        return false;
    }

    ::item* capture =
        item.getKeyboardCapture();

    // Leave OPTN to focused widgets (eg. Graph). EXIT releases focus first.
    if(capture != nullptr)
        return false;

    openScreenshotPrompt();

    casioEvent.type = KEYEV_NONE;
    casioEvent.key = 0;

    return true;
}


void GUI::processPendingScreenshot()
{
    if(!screenshotPending)
        return;

    screenshotPending = false;
    lastScreenshotPath.clear();

    casio_gui_screenshot::result status =
        casio_gui_screenshot::saveBmp(
            screenshotPendingName,
            &lastScreenshotPath);

    lastScreenshotSuccess =
        status ==
        casio_gui_screenshot::result::OK;

    if(screenshotResultCallback)
    {
        screenshotResultCallback(
            lastScreenshotSuccess,
            lastScreenshotPath);
    }
}


int GUI::createPage(const std::string& name, int backgroundColor)
{
    for(const auto& entry : pages)
    {
        if(entry.second.name == name)
            return entry.first;
    }

    int id = nextPageId++;
    pages.emplace(id, gui_page{id, name, true, backgroundColor});
    return id;
}

gui_page* GUI::getPage(int pageId)
{
    auto it = pages.find(pageId);
    return (it == pages.end()) ? nullptr : &it->second;
}

const gui_page* GUI::getPage(int pageId) const
{
    auto it = pages.find(pageId);
    return (it == pages.end()) ? nullptr : &it->second;
}

int GUI::getPageId(const std::string& name) const
{
    for(const auto& entry : pages)
    {
        if(entry.second.name == name)
            return entry.first;
    }

    return -1;
}

bool GUI::setPage(int pageId)
{
    gui_page* page = getPage(pageId);

    if(page == nullptr || !page->enabled)
        return false;

    currentPageId = pageId;
    item.setActivePageId(pageId);
    checkHover();
    return true;
}

bool GUI::setPage(const std::string& name)
{
    int pageId = getPageId(name);
    return pageId >= 0 && setPage(pageId);
}

bool GUI::setPageEnabled(int pageId, bool enabled)
{
    gui_page* page = getPage(pageId);

    if(page == nullptr)
        return false;

    if(!enabled && pageId == currentPageId)
        return false;

    page->enabled = enabled;
    return true;
}

bool GUI::setPageBackgroundColor(int pageId, int color)
{
    gui_page* page = getPage(pageId);

    if(page == nullptr)
        return false;

    page->backgroundColor = color;
    return true;
}

int GUI::getCurrentPageBackgroundColor() const
{
    const gui_page* page = getPage(currentPageId);
    return (page == nullptr) ? C_WHITE : page->backgroundColor;
}

int GUI::addItemToPage(int pageId, ::item* currentItem)
{
    if(currentItem == nullptr || getPage(pageId) == nullptr)
        return -1;

    currentItem->setPageId(pageId);
    return item.addItem(currentItem);
}

int GUI::addGlobalItem(::item* currentItem)
{
    if(currentItem == nullptr)
        return -1;

    currentItem->setGlobalPage();
    return item.addItem(currentItem);
}

bool GUI::moveItemToPage(int itemId, int pageId)
{
    if(pageId != ITEM_PAGE_GLOBAL && getPage(pageId) == nullptr)
        return false;

    return item.moveItemToPage(itemId, pageId);
}

void GUI::applyTheme(
    const item_theme& theme,
    bool updatePageBackgrounds)
{
    set_item_theme(theme);

    cursor.setNormalColor(
        theme.text.getRGB()
    );

    cursor.setFocusColor(
        theme.warning.getRGB()
    );

    item.applyTheme(theme);

    if(updatePageBackgrounds)
    {
        for(auto& entry : pages)
        {
            entry.second.backgroundColor =
                theme.background.getRGB();
        }
    }

    checkHover();
}

void GUI::setThemePreset(
    item_theme_preset preset,
    bool updatePageBackgrounds)
{
    applyTheme(
        make_item_theme(preset),
        updatePageBackgrounds
    );
}


void GUI::checkHover()
{
    if(!cursor.isVisible())
    {
        for(unsigned int i = 0; i < item.getItemsCount(); ++i)
        {
            auto* currentItem = item.getItems(i);

            if(currentItem != nullptr)
                currentItem->clearPointerState();
        }

        return;
    }

    int _x = cursor.x();
    int _y = cursor.y();

    // First remove hover from every item.
    for(unsigned int i = 0; i < item.getItemsCount(); ++i)
    {
        auto* currentItem = item.getItems(i);

        if(currentItem != nullptr)
            currentItem->clearHover();
    }

    // Only the top-most item under the cursor receives hover.
    // This makes z-order meaningful for overlapping controls.
    auto* topItem = item.getTopItemAt(_x, _y);

    if(topItem != nullptr)
        topItem->isHover(_x, _y);
}

void GUI::checkClick()
{
    if(!cursor.isVisible())
        return;

    for(unsigned int i = 0; i < item.getItemsCount(); ++i)
    {
        auto* currentItem = item.getItems(i);

        if(currentItem != nullptr)
            currentItem->isClick(casioEvent.key);
    }
}

void GUI::checkItemEvents()
{
    ::item* hoveredItem = nullptr;

    if(cursor.isVisible())
    {
        hoveredItem = item.getTopItemAt(
            cursor.x(),
            cursor.y()
        );
    }

    // A click on a focusable item transfers the common GUI focus to it.
    if(
        hoveredItem != nullptr &&
        casioEvent.type == KEYEV_DOWN &&
        casioEvent.key == KEY_EXE &&
        hoveredItem->wantsPointerFocus(
            cursor.x(),
            cursor.y()
        ))
    {
        item.setFocusById(hoveredItem->getId());
    }

    auto* keyboardCapture = item.getKeyboardCapture();

    // A focused control keeps receiving keyboard events even if the cursor is
    // hidden or no longer positioned over that control.
    if(keyboardCapture != nullptr && keyboardCapture != hoveredItem)
    {
        keyboardCapture->handleEvent(
            casioEvent.type,
            casioEvent.key
        );
    }

    // Pointer-dependent events remain disabled when the cursor is hidden.
    if(cursor.isVisible() && hoveredItem != nullptr)
    {
        hoveredItem->isClick(casioEvent.key);

        hoveredItem->handleEvent(
            casioEvent.type,
            casioEvent.key
        );

        hoveredItem->chckEvent(
            casioEvent.type,
            casioEvent.key
        );
    }

    if(casioEvent.type == KEYEV_UP)
    {
        for(unsigned int i = 0; i < item.getItemsCount(); ++i)
        {
            auto* currentItem = item.getItems(i);

            if(currentItem != nullptr)
                currentItem->isClick(-1);
        }
    }
}

void GUI::checkEvents()
{
    auto* keyboardCapture = item.getKeyboardCapture();

    // Modal items such as popups block application/global shortcuts while open.
    if(keyboardCapture != nullptr && keyboardCapture->isModal())
        return;

    switch(casioEvent.type)
    {
        case KEYEV_DOWN:
            guiEvent.triggerEvent(
                itemEvent::eventType::KEY_DOWN,
                casioEvent.key
            );
            break;

        case KEYEV_UP:
            guiEvent.triggerEvent(
                itemEvent::eventType::KEY_UP,
                casioEvent.key
            );
            break;

        case KEYEV_HOLD:
            guiEvent.triggerEvent(
                itemEvent::eventType::KEY_PRESS,
                casioEvent.key
            );
            break;

        default:
            break;
    }

    // Items such as the bottom F1..F6 menu listen globally without stealing
    // keyboard focus from textbox/numeric/slider controls.
    item.dispatchGlobalEvent(
        casioEvent.type,
        casioEvent.key
    );
}

bool GUI::runGUI()
{
    // A small periodic timer wakes getkey_opt() even when the user does not
    // press a key. This is required for invisible item_time objects and also
    // keeps animations/GUI state responsive without a busy loop.
    volatile int frameTick = 0;

    int frameTimer = timer_configure(
        TIMER_ANY,
        16667, // ~60 Hz
        GINT_CALL_SET(&frameTick)
    );

    // TextBox handles SHIFT/ALPHA itself. Disable gint modifier consumption
    // so KEY_SHIFT and KEY_ALPHA are delivered as regular key events.
    constexpr int guiGetKeyOptions =
        GETKEY_DEFAULT &
        ~GETKEY_MOD_SHIFT &
        ~GETKEY_MOD_ALPHA;

    if(frameTimer >= 0)
        timer_start(frameTimer);

    item.update(rtc_ticks());
    checkHover();

    dclear(getCurrentPageBackgroundColor());
    draw();
    dupdate();

    while(true)
    {
        frameTick = 0;

        if(frameTimer >= 0)
        {
            casioEvent = getkey_opt(
                guiGetKeyOptions,
                &frameTick
            );
        }
        else
        {
            // Rare fallback if no hardware timer can be allocated.
            casioEvent = getkey_opt(
                guiGetKeyOptions,
                nullptr
            );
        }

        item.update(rtc_ticks());

        if(
            casioEvent.type == KEYEV_DOWN &&
            casioEvent.key == KEY_EXIT)
        {
            // EXIT is only a widget-level back/close/defocus key.
            // There is deliberately no break from runGUI() here. MENU is left
            // to the calculator/gint environment to leave or switch the add-in.
            if(item.handleSystemExit())
            {
                casioEvent.type = KEYEV_NONE;
                casioEvent.key = 0;
            }
        }

        checkScreenshotShortcut();

        auto* keyboardCapture = item.getKeyboardCapture();

        if(
            casioEvent.type != KEYEV_NONE &&
            cursor.isVisible() &&
            keyboardCapture == nullptr &&
            (casioEvent.type == KEYEV_DOWN || casioEvent.type == KEYEV_HOLD))
        {
            cursor.move(casioEvent.key);
        }

        checkHover();

        dclear(getCurrentPageBackgroundColor());

        if(casioEvent.type != KEYEV_NONE)
        {
            checkEvents();
            checkItemEvents();
        }

        // A callback can switch page, move/hide the cursor or open a popup.
        checkHover();

        draw();

        // The naming popup has already closed on submit. Capture the rebuilt
        // application frame so the popup itself is not saved.
        processPendingScreenshot();

        dupdate();
    }

    if(frameTimer >= 0)
        timer_stop(frameTimer);

    return true;
}

