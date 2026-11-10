#pragma once
#include "Application.h"
#include "StudioList.h"
#include "ClientNode.h"

class String;
class FileTree;
class Document;
class iStream;
class Bytes;

class StudioFrame : public Application {
public:
    explicit StudioFrame(HINSTANCE hInstance);
    ~StudioFrame() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void onSetFont(HFONT font, bool redraw) override;
    bool closeAll() override;
    bool resolvePath(String* path) override;
    int  loadFromFile(iStream* in, String* path, bool decrypt) override;
    int  saveToFile(Bytes* data, String* path, bool encrypt) override;
    void* vfunc_154() override;
    String* getWorkspacePath(String* out) override;
    void onStartup() override;
    Document* createView(String* ext, FileTree* node, bool reload) override;
    void onMenuSelect(HMENU menu, UINT item, UINT flags) override;
    void onOptions() override;
    virtual bool isArchiveReadOnly();
    virtual bool isWorkspaceEmpty();
    virtual bool connectServer(String* path);
    virtual String* getServerPath(String* out);
    virtual void* getSqlConnection();
    virtual FileTree* findFile(const wchar_t* path);
    virtual FileTree* markFileModified(const wchar_t* path);

    bool open(int a, int b, int c);

    StudioList m_list;
    ClientNode m_client;
};

extern StudioFrame* g_studioFrame;

int studioLoadStream(void* stream, FileTree* node, bool decrypt);
int studioSaveStream(void* stream, FileTree* node, int unused);
int studioLoadFile(void* stream, String* path);
int studioSaveFile(void* stream, String* path);
int studioFillBackground(int a1, int a2, int a3, int a4);
void* studioItemData();
bool __stdcall isWorkspaceFolder(const wchar_t* path);
