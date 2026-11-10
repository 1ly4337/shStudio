#include "p05_common.h"
#include <shlwapi.h>
#include "StudioFrame.h"
#include "String.hpp"
#include "FileTree.h"
#include "FolderRoot.h"
#include "ServerNode.h"
#include "SqlSvrNode.h"
#include "AES.h"
#include "SEED.h"
#include "ArchiveTree_g3.h"
#include "AboutBox.h"
#include "AlertDial.h"
#include "ChooseCodepageDialog.h"
#include "FileDialogs.h"
#include "Time.hpp"
#include "PackArchiveDialog.h"
#include "ArchiveWriter.h"
#include "Options.h"
#include "LoadServerFiles.h"
#include "LoadSQLDefs.h"
#include "ModelFiles.h"
#include "CharActions.h"
#include "MonsterModels.h"
#include "AniFile.h"
#include "Effect.h"
#include "Bytes.h"
#include "iStream.hpp"
#include "TextView.h"
#include "StdTextView.h"
#include "WeatherView.h"
#include "ClothView.h"
#include "ThreeDxView.h"
#include "VaniView.h"
#include "DungeonView.h"
#include "WorldView.h"
#include "MlxView.h"
#include "ModuleView.h"
#include "AltView.h"
#include "PcView.h"
#include "ItmView.h"
#include "MltView.h"
#include "MonView.h"
#include "TacticsView.h"
#include "SVMapViewDoc.h"
#include "CashView.h"
#include "ItemView.h"
#include "ItemSetView.h"
#include "MonsterView.h"
#include "NpcView.h"
#include "NpcSkillView.h"
#include "ToonSkillView.h"
#include "DualLayerView.h"
#include "TransformWeaponView.h"
#include "EffectView.h"
#include "DBTextView.h"

#define SF_ROOT(f)          ((FolderRoot*)((uint8_t*)(f) + 0x178))
#define SF_SOURCE(f)        FIELD(void*, f, 0x170)

static inline aStream* asAStream(void* streamWithVbptr)
{
    return (aStream*)((uint8_t*)streamWithVbptr + (*(int**)streamWithVbptr)[1]);
}

StudioFrame* g_studioFrame = 0;

WCHAR g_caption[24] = { 0x0982, 0x4356, 0x4BAB, 0x7A30, 0x860C, 0x327E, 0x6DBE, 0xE415, 0x5F43, 0x2B16, 0x518E, 0xB83C, 0x6913, 0xA426, 0xDBAE, 0x0AE9, 0xBCD3, 0xE39E, 0x0023, 0xC3D2, 0x47D0, 0x9A12, 0x363E, 0x4B43 };
WCHAR g_productName[16] = { 0x7CE8, 0x37F1, 0xA7D1, 0xFB1D, 0x0BED, 0x111E, 0x9B0C, 0xFBC2, 0x2FBB, 0xDF80, 0x4B1F, 0xD24E, 0x76E5, 0x1DA9, 0xE1E6, 0xA7F4 };
COMDLG_FILTERSPEC g_workspaceFilter[1] = { { L"Archive Shaiya", L"*.saf" } };

StudioFrame::StudioFrame(HINSTANCE hInstance)
    : Application(hInstance),
      m_list(this, &m_client),
      m_client(&m_list, 0)
{
    OleInitialize(0);
    g_studioFrame = this;
    g_layoutCodePage = g_preferences.getInt(L"Layout", L"code page", 0);
}

StudioFrame::~StudioFrame()
{
    OleUninitialize();
}

void* StudioFrame::vfunc_154()
{
    return SF_ROOT(this);
}

String* StudioFrame::getWorkspacePath(String* out)
{
    *out = getContentFolder();
    return out;
}

bool StudioFrame::isWorkspaceEmpty()
{
    return m_client.isClosed() == 0;
}

FileTree* StudioFrame::markFileModified(const wchar_t* path)
{
    FileTree* file = findFile(path);
    if (file)
        FIELD(int, file, 0xFC) = 2;
    return file;
}

bool StudioFrame::isArchiveReadOnly()
{
    ContentProvider* provider = g_contentProvider;
    if (provider)
        return provider->isArchive();
    return false;
}

bool StudioFrame::connectServer(String* path)
{
    if (!path->m_length)
        return false;
    Node* node = m_list.getItemNode(m_list.m_serverItem);
    ServerNode* server = dynamic_cast<ServerNode*>(node);
    return server && server->connect(&m_list, path) == 0;
}

String* StudioFrame::getServerPath(String* out)
{
    new (out) String();
    Node* node = m_list.getItemNode(m_list.m_serverItem);
    ServerNode* server = dynamic_cast<ServerNode*>(node);
    if (server && FIELD(int, server, 0x40)) {
        *out = FIELD(String, server, 0x44);
        out->toParentPath();
        out->toParentPath();
    }
    return out;
}

void* StudioFrame::getSqlConnection()
{
    Node* node = m_list.getItemNode(m_list.m_sqlItem);
    SqlSvrNode* sql = dynamic_cast<SqlSvrNode*>(node);
    if (!sql)
        return 0;
    uint8_t* conn = FIELD(uint8_t*, sql, 0x18);
    if (!conn || !conn[24])
        return 0;
    return conn;
}

FileTree* StudioFrame::findFile(const wchar_t* path)
{
    if (path)
        return SF_ROOT(this)->findFile(path);
    return 0;
}

int studioFillBackground(int a1, int a2, int a3, int a4)
{
    DWORD color = panelBackgroundColor();
    return g_studioFrame->m_client.m_clientData.renderItem((char)a1, a2, color, a3, a4);
}

void* studioItemData()
{
    ClientData& data = g_studioFrame->m_client.m_clientData;
    data.loadSData(&data.m_items, L"Item\\item.sdata", L"BinarySData\\DBItemData.sdata", (int)(INT_PTR)&data);
    return &data.m_items;
}

bool StudioFrame::closeAll()
{
    if (!Application::closeAll())
        return false;
    HMENU menu = m_hMenu;
    EnableMenuItem(menu, 0xE105, MF_GRAYED);
    EnableMenuItem(menu, 0xE106, MF_GRAYED);
    FIELD(int, this, 0xB8) = 0;
    FIELD(char, this, 0xBC) = 0;
    m_list.invalidate(true);
    UpdateWindow(m_list.m_hWnd);
    m_client.unload();
    return true;
}

bool StudioFrame::resolvePath(String* path)
{
    unsigned int len = path->m_length;
    if (SF_SOURCE(this)) {
        unsigned int i = 0;
        if (!len)
            return SF_ROOT(this)->findFile(path->m_buffer) != 0;
        for (const wchar_t* p = path->m_buffer; *p != L'*'; ++p) {
            if (++i >= len)
                return SF_ROOT(this)->findFile(path->m_buffer) != 0;
        }
        String dir(*path);
        dir.removeFileName();
        String spec;
        spec.append(PathFindFileNameW(path->m_buffer), -1);
        bool found;
        FolderTree* folder = SF_ROOT(this)->findFolderPath(dir.m_buffer);
        if (folder) {
            FolderTree::FilesTree* files = &folder->m_files;
            int count = files->size();
            found = false;
            for (int k = 0; k < count; ++k) {
                uint8_t* file = (uint8_t*)files->get(k);
                if (file && PathMatchSpecW(FIELD(LPCWSTR, file, 0x34), spec.m_buffer)) {
                    String full;
                    full = dir + *(String*)(file + 0x30);
                    *path = full;
                    found = true;
                    break;
                }
            }
        } else {
            found = Application::resolvePath(path);
        }
        return found;
    }
    unsigned int i = 0;
    if (!len)
        return PathFileExistsW(path->m_buffer) != 0;
    const WCHAR* s = path->m_buffer;
    for (const WCHAR* p = s; *p != L'*'; ++p) {
        if (++i >= len)
            return PathFileExistsW(path->m_buffer) != 0;
    }
    if ((int)i <= 0)
        return PathFileExistsW(path->m_buffer) != 0;
    WIN32_FIND_DATAW fd;
    memset(&fd, 0, sizeof(fd));
    HANDLE h = FindFirstFileW(s, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return false;
    FindClose(h);
    return true;
}

static bool dataFileHeaderIsEncrypted(const BYTE* header)
{
    return memcmp(header, g_dataFileSignature, 16) == 0;
}

static bool dataFileIsEncrypted(const wchar_t* path)
{
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    if (file == INVALID_HANDLE_VALUE)
        return false;
    BYTE header[0x40];
    DWORD read = 0;
    LARGE_INTEGER size;
    bool encrypted = GetFileSizeEx(file, &size) && size.QuadPart >= 0x40 && (size.QuadPart & 0xF) == 0 &&
                     ReadFile(file, header, sizeof(header), &read, 0) && read == sizeof(header) &&
                     dataFileHeaderIsEncrypted(header);
    CloseHandle(file);
    return encrypted;
}

int StudioFrame::loadFromFile(iStream* in, String* path, bool decrypt)
{
    ContentProvider* provider = g_contentProvider;
    void* entry = SF_ROOT(this)->findFile(path->m_buffer);
    if (provider && entry)
        return provider->readFile((FileTree*)entry, in, decrypt);
    int err = asAStream(in)->load(path->m_buffer);
    if (err)
        return err;
    if (!decrypt)
        return err;
    char tmp;
    if (!sub_418C00(asAStream(in), &tmp, 0))
        return E_FAIL;
    return err;
}

int StudioFrame::saveToFile(Bytes* data, String* path, bool encrypt)
{
    ContentProvider* provider = g_contentProvider;
    void* entry = SF_ROOT(this)->findFile(path->m_buffer);
    if (encrypt)
        encrypt = provider && entry ? dataFileHeaderIsEncrypted(FileTree_key((FileTree*)entry))
                                    : dataFileIsEncrypted(path->m_buffer);
    if (provider && entry) {
        void* out = data ? (uint8_t*)data + 8 : 0;
        return provider->writeFile((FileTree*)entry, (iStream*)out, encrypt);
    }
    if (encrypt) {
        uint8_t header[0x40];
        memset(header, 0, sizeof(header));
        if (!sub_418E60(data ? asAStream(data) : 0, header))
            return E_FAIL;
        Bytes file;
        static_cast<oStream&>(file).put(header, 64);
        file.append(*data);
        int res = asAStream(&file)->save(path->m_buffer);
        return res;
    }
    return asAStream(data)->save(path->m_buffer);
}

int studioLoadStream(void* stream, FileTree* node, bool decrypt)
{
    if (g_contentProvider)
        return g_contentProvider->readFile(node, (iStream*)stream, decrypt);
    int err = asAStream(stream)->load(FIELD(const wchar_t*, node, 0x14));
    if (err)
        return err;
    if (!decrypt)
        return err;
    if (!((String*)((uint8_t*)node + 0x30))->String::sub_4121F0(L"DBItemSellData.sdata"))
        return err;
    char tmp = 0;
    if (!sub_418C00(asAStream(stream), &tmp, (uint8_t*)node + 0x100))
        return E_FAIL;
    return err;
}

int studioSaveStream(void* stream, FileTree* node, int )
{
    if (g_contentProvider)
        return g_contentProvider->writeFile(node, (iStream*)stream, false);
    return asAStream(stream)->save(FIELD(const wchar_t*, node, 0x14));
}

int studioLoadFile(void* stream, String* path)
{
    uint8_t* frame = (uint8_t*)g_studioFrame;
    void* entry = SF_ROOT(frame)->findFile(path->m_buffer);
    if (entry) {
        if (g_contentProvider)
            return g_contentProvider->readFile((FileTree*)entry, (iStream*)stream, false);
        return 0x80004003;
    }
    Bytes bytes;
    int result;
    if (((StudioFrame*)frame)->loadFromFile(&bytes, path, false)) {
        result = 2;
    } else {
        aStream* src = asAStream(&bytes);
        asAStream(stream)->assign(src->m_data, src->m_size);
        result = 0;
    }
    return result;
}

int studioSaveFile(void* stream, String* path)
{
    uint8_t* frame = (uint8_t*)g_studioFrame;
    void* entry = SF_ROOT(frame)->findFile(path->m_buffer);
    if (!entry)
        return asAStream(stream)->save(path->m_buffer);
    if (g_contentProvider)
        return g_contentProvider->writeFile((FileTree*)entry, (iStream*)stream, false);
    return 0x80004003;
}

bool StudioFrame::onCreate()
{
    bool result = Application::onCreate();
    if (!result)
        return result;
    AES aes(128);
    aes.setKey(&unk_5467EC, 16);
    aes.decrypt(&g_productName, 32, &g_productName);
    aes.setKey(&unk_5467DC, 16);
    VCALL(&aes, 24, void, WCHAR*, int, WCHAR*)(&aes, g_caption, 48, g_caption);
    RECT rc;
    RECT* r = getClientRect(&rc);
    bool ok = m_list.Window::create(m_hWnd, (HMENU)0x65, r, 0x50000000, 0x100, 0);
    return ok;
}

void StudioFrame::onOptions()
{
    Options dlg(this);
    if (dlg.create(m_hWnd))
        dlg.doModal();
}

void StudioFrame::onStartup()
{
    LoadServerFiles* t1 = new LoadServerFiles(this);
    HANDLE h = CreateThread(0, t1->m_stackSize, Thread::threadProc, t1,
                            t1->m_creationFlags, &t1->m_threadId);
    t1->m_handle = h;
    if (!h)
        Thread::destroy(t1);
    LoadSQLDefs* t2 = new LoadSQLDefs(this);
    h = CreateThread(0, t2->m_stackSize, Thread::threadProc, t2,
                     t2->m_creationFlags, &t2->m_threadId);
    t2->m_handle = h;
    if (!h)
        Thread::destroy(t2);
}

void StudioFrame::onSetFont(HFONT font, bool )
{
    if (font)
        m_list.setFont(font);
}

bool __stdcall isWorkspaceFolder(const wchar_t* src)
{
    String path;
    path.append(src, -1);
    path << L"\\Item\\";
    bool ok;
    DWORD attr = GetFileAttributesW(path.m_buffer);
    if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY) == 0) {
        ok = false;
    } else {
        path = src;
        path << L"\\cash.sdata";
        ok = GetFileAttributesW(path.m_buffer) != INVALID_FILE_ATTRIBUTES;
    }
    return ok;
}

void StudioFrame::onMenuSelect(HMENU , UINT id, UINT )
{
    switch (id) {
    case 0xE140: setStatusText(L"Displays the version of the tool.", 0); return;
    case 0xE100: setStatusText(L"Expand a data.saf archive to create a new workspace.", 0); return;
    case 0xE101: setStatusText(L"Select an existing workspace and load its content.", 0); return;
    case 0xE110: setStatusText(L"Select an existing data.saf archive and load its content.", 0); return;
    case 0xE102: setStatusText(L"Close the opened archive or workspace.", 0); return;
    }
    const wchar_t* text = L"Pack the current workspace to create a new archive.";
    if (id != 0xE105)
        text = 0;
    setStatusText(text, 0);
}

bool StudioFrame::onCommand(UINT code, HWND ctrl, UINT id)
{
    UINT c = code;
    if (code == 1 && !ctrl) {
        HWND fg = GetForegroundWindow();
        if (Popup::findDocPopup(fg))
            return Application::onCommand(1, 0, id);
        c = 1;
    }
    if ((c | 1) != 1 || ctrl)
        return Application::onCommand(c, ctrl, id);

    if (id == 0xE140) {
        AboutBox about;
        if (about.create(m_hWnd))
            about.doModal();
        return true;
    }
    if (id == 0xE100) {
        if (closeAll()) {
            setStatusText(0, 0);
            onInitMenu(m_hMenu);
            if (m_client.createWorkspace()) {
                onInitMenu(m_hMenu);
                return true;
            }
        }
        return true;
    }
    if (id != 0xE101 && id != 0xE110) {
        if (id == 0xE102) {
            SetCursor(hCursor);
            closeAll();
            SetCursor(g_arrowCursor);
            return true;
        }
        if (id != 0xE105)
            return Application::onCommand(c, ctrl, id);
        ContentProvider* provider = g_contentProvider;
        if (!provider || provider->isArchive())
            return true;
        const wchar_t* wsPath = provider->getDirectory();
        String workspace(wsPath, -1);
        String target(workspace);
        target.toParentPath();
        target << "data.saf";
        DWORD filter = 1;
        bool done;
        if (FileDialogs_save(m_hWnd, &target, L".saf", g_workspaceFilter, 1, &filter,
                       L"Save the workspace as")) {
            done = false;
        } else if (target.String::startsWith(workspace)) {
            AlertDial::alert(m_hWnd,
                       L"The archive file can not be saved under its own hierarchy.\nChoose a parent folder instead.",
                       0, 16);
            done = false;
        } else {
            HANDLE file = CreateFileW(target.m_buffer, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                                      FILE_ATTRIBUTE_NORMAL, 0);
            if (file == INVALID_HANDLE_VALUE) {
                AlertDial::alert(m_hWnd,
                           L"The archive file can not be created.\nCheck path & access right.", 0, 16);
                done = false;
            } else {
                String index(target);
                wchar_t* indexName = index.m_buffer;
                if (index.m_length)
                    indexName[index.m_length - 1] = L'h';
                _wremove(indexName);
                {
                    ArchiveWriter out(file);
                    PackArchiveDialog dlg(SF_ROOT(this), true, &out);
                    if (dlg.create(m_hWnd)) {
                        if (dlg.doModal() == 1)
                            asAStream(&out.m_index)->save(indexName);
                        else
                            _wremove(target.m_buffer);
                    }
                    done = true;
                    if (out.m_file != INVALID_HANDLE_VALUE)
                        CloseHandle(out.m_file);
                }
            }
        }
        return done;
    }

    if (!closeAll())
        return true;
    setStatusText(0, 0);
    onInitMenu(m_hMenu);
    const wchar_t* key = L"last";
    if (id != 0xE101)
        key = L"file";
    String location = g_preferences.getString(L"Workspace", key, 0);
    bool chosen;
    if (id == 0xE101)
        chosen = FileDialogs_selectFolder(m_hWnd, &location, L"Select a workspace to open", 0, true,
                                          isWorkspaceFolder);
    else
        chosen = FileDialogs_open(m_hWnd, &location, L"Shaiya Archive",
                            L"Choose an archive to open", 0) != 0;
    if (!chosen) {
        setStatusText(0, 0);
        return true;
    }
    g_preferences.writeString(L"Workspace", key, location.m_buffer);
    String options = g_preferences.getString(L"Workspace", location.m_buffer, 0);
    int codePage;
    bool haveCp = false;
    if (options.m_length > 0) {
        int pos = options.indexOf(L"&cp=");
        if (pos > 0) {
            String tail = options.mid(pos + 4);
            codePage = _wtoi(tail.m_buffer);
            if (codePage != (int)0x80000000 && codePage != 0x7FFFFFFF && codePage)
                haveCp = true;
        }
    }
    if (!haveCp) {
        codePage = ChooseCodepageDialog::run(m_hWnd, location.m_buffer);
        if (codePage < 0)
            return true;
        options.clear();
        Time now;
        String stamp = now.toAsn1();
        String* s = &stamp;
        String* o = &options.append(s->m_buffer, s->m_length);
        o = &(*o << L"&cp=");
        *o << (unsigned long)codePage;
        g_preferences.writeString(L"Workspace", location.m_buffer, options.m_buffer);
    }
    ClientNode* client = m_list.m_client;
    if (client->open(location, codePage)) {
        onInitMenu(m_hMenu);
        if (id == 0xE101)
            EnableMenuItem(m_hMenu, 0xE105, MF_ENABLED);
    }
    return true;
}

Document* StudioFrame::createView(String* ext, FileTree* node, bool reload)
{
    Document* view = 0;
    String name(*(String*)((uint8_t*)node + 0x30));
    for (unsigned int i = 0; i < (unsigned int)name.m_length; ++i) {
        wchar_t* ch = &name.m_buffer[i];
        if ((unsigned short)(*ch - L'A') <= 0x19u)
            *ch += 32;
    }
    bool dbFiles;
    {
        String primary = g_preferences.getString(L"SData", L"Primary", L"undefined");
        String* p = &primary;
        dbFiles = p->m_length == 7 && _wcsnicmp(p->m_buffer, L"DBfiles", 7) == 0;
    }
#define EXT_IS(s)  (*ext == (s))
#define NAME_IS(s) name.equalsNoCase(s)

    if (ext->m_length == 4 && !_wcsnicmp(ext->m_buffer, L".lua", 4)) {
        view = new TextView(node, true);
    } else if (EXT_IS(L".alt")) {
        CharActions* alt = 0;
        if (reload)
            alt = m_client.m_clientData.m_charActions.load(node, &m_list);
        view = new AltView(node, alt);
    } else if (EXT_IS(L".wtr")) {
        view = new WeatherView(node);
    } else if (EXT_IS(L".ctl") || EXT_IS(L".dat")) {
        view = new ClothView(node);
    } else if (EXT_IS(L".3dc") || EXT_IS(L".3de") || EXT_IS(L".3do")) {
        view = new ThreeDxView(node, 0);
    } else if (EXT_IS(L".vani")) {
        view = new VaniView(node);
    } else if (EXT_IS(L".smod")) {
        view = new ModuleView(node);
    } else if (EXT_IS(L".pc")) {
        view = new PcView(node);
    } else if (EXT_IS(L".itm")) {
        ItemModels* models = 0;
        if (reload) {
            char flag = (char)name.removeExtension().toInt();
            models = m_client.m_clientData.getItemModels(flag, 0);
        }
        view = new ItmView(node, models);
    } else if (EXT_IS(L".mlt")) {
        ModelsMLT* models = reload ? m_client.m_clientData.getCharModelsByName(name) : 0;
        view = new MltView(node, models);
    } else if (EXT_IS(L".mlx")) {
        ModelsMLX* models = m_client.m_clientData.getMLXByName(name);
        if (models)
            view = new MlxView(node, models, &m_client.m_clientData.m_items);
    } else if (EXT_IS(L".mon")) {
        MonsterModels* mon = 0;
        if (reload)
            mon = m_client.m_clientData.m_monsterModels.load(node, &m_list);
        view = new MonView(node, mon);
    } else if (EXT_IS(L".tol")) {
        AlertDial::alert(m_hWnd,
                   L".TOL files contain a list of NPC shape and animations.\nThese files are deprecated and replaced by .MON files.",
                   0, 64);
        return 0;
    } else if (EXT_IS(L".zon")) {
        TacticsZones* zones = reload ? &m_client.m_clientData.m_tacticsZones : 0;
        view = new TacticsView(node, zones);
    } else if (EXT_IS(L".wld")) {
        String copy(name);
        copy.removeExtension();
        int flag = copy.toInt();
        view = new WorldView(node, flag);
    } else if (EXT_IS(L".dg")) {
        view = new DungeonView(node);
    } else if (EXT_IS(L".svmap")) {
        view = new SVMapViewDoc(node);
    } else if (NAME_IS(L"Cash.sdata") || NAME_IS(L"DBItemSellData.sdata")) {
        Serializable* data = 0;
        if (reload) {
            if (dbFiles ? NAME_IS(L"DBItemSellData.sdata") : NAME_IS(L"Cash.sdata"))
                data = &m_client.m_clientData.m_cashProducts;
        }
        view = new CashView(node, data);
    } else if (NAME_IS(L"Item.sdata") || NAME_IS(L"DBItemData.sdata")) {
        Serializable* data = 0;
        if (reload) {
            if (dbFiles ? NAME_IS(L"DBItemData.sdata") : NAME_IS(L"Item.sdata"))
                data = &m_client.m_clientData.m_items;
        }
        view = new ItemView(node, data);
    } else if (NAME_IS(L"SetItem.sdata") || NAME_IS(L"DBSetItemData.sdata")) {
        Serializable* data = 0;
        if (reload) {
            if (dbFiles ? NAME_IS(L"DBSetItemData.sdata") : NAME_IS(L"SetItem.sdata"))
                data = &m_client.m_clientData.m_itemSets;
        }
        view = new ItemSetView(node, data);
    } else if (NAME_IS(L"Monster.sdata") || NAME_IS(L"DBMonsterData.sdata")) {
        Serializable* data = 0;
        if (reload) {
            if (dbFiles ? NAME_IS(L"DBMonsterData.sdata") : NAME_IS(L"Monster.sdata"))
                data = &m_client.m_clientData.m_monsters;
        }
        view = new MonsterView(node, data);
    } else if (NAME_IS(L"NpcQuest.sdata")) {
        view = new NpcView(node, reload ? static_cast<Serializable*>(&m_client.m_clientData.m_quests) : 0);
    } else if (NAME_IS(L"NpcSkill.sdata") || NAME_IS(L"DBNpcSkillData.sdata")) {
        Serializable* data = 0;
        if (reload) {
            if (dbFiles ? NAME_IS(L"DBNpcSkillData.sdata") : NAME_IS(L"NpcSkill.sdata"))
                data = &m_client.m_clientData.m_mobsSkills;
        }
        view = new NpcSkillView(node, data);
    } else if (NAME_IS(L"Skill.sdata") || NAME_IS(L"DBSkillData.sdata")) {
        Serializable* data = 0;
        if (reload) {
            if (dbFiles ? NAME_IS(L"DBSkillData.sdata") : NAME_IS(L"Skill.sdata"))
                data = &m_client.m_clientData.m_toonSkills;
        }
        view = new ToonSkillView(node, data);
    } else if (NAME_IS(L"DualLayerClothes.sdata") || NAME_IS(L"DBDualLayerClothesData.sdata")) {
        view = new DualLayerView(node, L"Dual Layer Clothes");
    } else if (NAME_IS(L"DBTransformModelData.sdata")) {
        view = new DualLayerView(node, L"Transform Model");
    } else if (NAME_IS(L"DBTransformWeaponModelData.sdata")) {
        view = new TransformWeaponView(node);
    } else if (NAME_IS(L"PriestTalk.sdata")) {
        AlertDial::alert(m_hWnd,
                   L"PriestTalk.sdata contains several strings always encoded in Korean (CP 949). All files seen contain errors on the length of certain sentences. This file is very likely not used by the game client.",
                   0, 64);
    } else if (EXT_IS(L".ani")) {
        AniFile ani;
        Bytes data;
        loadFromFile((iStream*)&data, (String*)((uint8_t*)node + 0x10), false);
        ani.read(data);
        String text;
        ani.dump(text);
        OutputDebugStringW(text.m_buffer);
        StdTextView* tv = new StdTextView();
        view = tv;
        tv->setContent(text);
    } else if (EXT_IS(L".eft")) {
        iStream in(0, 0);
        int err;
        if (g_contentProvider)
            err = g_contentProvider->readFile(node, &in, false);
        else
            err = asAStream(&in)->load(FIELD(const wchar_t*, node, 0x14));
        if (!err) {
            Effect effect;
            effect.read(in, 'file');
            String text;
            int tag = 'dump';
            if (GetKeyState(VK_LCONTROL) & 0x8080)
                tag = 'tabs';
            effect.toText(text, tag);
            TextView* tv = new TextView(node, false);
            tv->setText(text);
            view = tv;
        }
    } else if (EXT_IS(L".seff")) {
        view = new EffectView(node, 0);
    } else {
        PathMatchSpecW(FIELD(LPCWSTR, node, 0x34), L"DB*Text_???.sdata");
        if (PathMatchSpecW(FIELD(LPCWSTR, node, 0x34), L"DB*Text_*.sdata"))
            view = new DBTextView(node);
    }
#undef EXT_IS
#undef NAME_IS
    return view;
}
