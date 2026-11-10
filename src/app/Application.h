#pragma once
#include "Popup.h"
#include "Documents.h"

class Doc;
class FileTree;
class String;
class iStream;
class Bytes;
class Document;

class Application : public Popup
{
public:
    explicit Application(HINSTANCE hInstance);
    ~Application() override;

    LPCWSTR getClassName() override;
    DWORD getExStyle() override;
    void onDestroy() override;
    void onActivate(bool active) override;
    bool onClose() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onDropFile(LPCWSTR path, HWND target, int targetId) override;
    bool onTimer(UINT_PTR id) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

    virtual int run();
    virtual Doc* findDocument(const wchar_t* path);
    virtual void addDocument(Doc* doc);
    virtual void updateDocument(FileTree* node);
    virtual void removeDocument(Doc* doc);
    virtual bool openFile(LPCWSTR path);
    virtual bool showDocument(Doc* doc, LPCWSTR name);
    virtual bool openDocument(FileTree* node, LPCWSTR path, bool reload);
    virtual bool reopenDocument(FileTree* node);
    virtual bool closeAll();
    virtual bool resolvePath(String* path);
    virtual int loadFromFile(iStream* in, String* path, bool decrypt);
    virtual int saveToFile(Bytes* data, String* path, bool encrypt);
    virtual void* vfunc_154() { return 0; }
    virtual String* getWorkspacePath(String* out);
    virtual void adjustInitialSize(SIZE* size) {}
    virtual void onStartup() {}
    virtual Document* createView(String* ext, FileTree* node, bool reload);
    virtual void onMenuSelect(HMENU menu, UINT item, UINT flags);
    virtual void onOptions() {}

public:
    bool      m_active;
    HACCEL    m_hAccel;
    Documents m_documents;
};
static_assert(sizeof(Application) == 0x68, "Application size");

INT_PTR CALLBACK Application_aboutDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

extern HINSTANCE   hModule;
extern String      g_appName;
extern HCURSOR     g_arrowCursor;
extern HCURSOR     g_handCursor;
extern HCURSOR     hCursor;
extern HWND        g_hMainWnd;
extern Application* g_mainFrame;
extern HBRUSH      g_aboutBrush;
extern const wchar_t* g_lastLoadError;
extern ULONG_PTR   g_gdiplusToken;
