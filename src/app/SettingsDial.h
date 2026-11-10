#pragma once
#include "Modal.h"
#include "ComboBox.h"
#include "ptrArray.h"
#include "OptionPanel.h"

class OptionPanel;

class SettingsDial : public Modal
{
public:
    explicit SettingsDial(void* owner);
    ~SettingsDial() override;
    bool create(HWND parent);

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onNotify(NMHDR* hdr, LRESULT* result) override;
    int mapCommandToResult(int id) override;

    virtual LPCWSTR getTitle();
    virtual SIZE* getDialogSize(SIZE* out);
    virtual bool createPanels() = 0;
    virtual bool addPanel(OptionPanel* panel, LPCWSTR name);
    virtual bool selectPanel(int key, bool byId);

public:
    ComboBox               m_pages;
    HWND                   m_hUpDown;
    ptrArray<OptionPanel*> m_panels;
    int                    m_current;
    void*                  m_settings;
};
static_assert(sizeof(SettingsDial) == 0xF8, "SettingsDial size");
