#pragma once
#include <windows.h>
#include "Modal.h"
#include "String.hpp"
#include "ComboBox.h"
#include "EditText.h"
#include "Button.h"

class MsSQLserver;

class SqlConnectDialog : public Modal
{
public:
    explicit SqlConnectDialog(MsSQLserver* server);
    virtual ~SqlConnectDialog();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void onShowDialog() override;
    int mapCommandToResult(int id) override;

public:
    MsSQLserver* m_server;
    String   m_driver;
    String   m_serverName;
    String   m_user;
    String   m_password;
    bool     m_save;
    ComboBox m_driverCombo;
    EditText m_serverEdit;
    EditText m_userEdit;
    EditText m_passwordEdit;
    Button   m_saveCheck;
};
static_assert(sizeof(SqlConnectDialog) == 0x18C, "SqlConnectDialog size");
