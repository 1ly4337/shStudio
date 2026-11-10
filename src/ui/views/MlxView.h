#pragma once
#include "ModelView.h"
#include "MlxList.h"
#include "ListGears.h"
#include "EditText.h"
#include "ComboBox.h"
#include "Button.h"

class ModelsMLX;

class MlxView : public ModelView {
public:
    MlxView(FileTree* node, ModelsMLX* models, void* items);
    ~MlxView() override;

    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wc) override;
    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onNotify(NMHDR* hdr, LRESULT* result) override;
    void onSize(WPARAM type, int cx, int cy) override;
    bool vfunc_FC(int msg) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;
    SIZE* getDefaultSize(SIZE* out, SIZE def) override;
    int  getMenuId() override;
    HRESULT store() override;
    void selectModel(int index) override;
    void editModel(int index) override;
    void placeView3D() override;
    int  loadMltFiles(const String& prefix);
    int  saveMltFiles(const String& prefix);

    ModelsMLX* m_models;
    wchar_t    m_raceCode[5];
    int        m_race;
    MlxList    m_list;
    HWND       m_labels[6];
    ListGears  m_gears[4];
    EditText   m_name;
    EditText   m_3dc[4];
    EditText   m_dds[4];
    ComboBox   m_alpha[4];
    Button     m_btnUpdate;
    Button     m_btnAppend;
    Button     m_btnRename;
    int        m_b24;
};
static_assert(sizeof(MlxView) == 0xB28, "MlxView size");
