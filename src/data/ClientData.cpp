#include "ClientData.h"
#include "ContentProvider.h"
#include "CountryNames.h"
#include "BitmapsID.h"
#include "FolderRoot.h"
#include "TreeList.h"
#include "Model3D.h"
#include "Window.h"
#include "Preferences.h"
#include "Bytes.h"
#include "iStream.hpp"
#include "StudioFrame.h"
#include "Data_Item.h"
#include "MonsterModel.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g2.h"
#include <shlwapi.h>
#include <commctrl.h>
#include <cwchar>
#include <cstring>

static inline int& fileTreeItem(FileTree* f) { return FIELD(int, f, 0x08); }
static inline int& fileUsed(FileTree* f)     { return FIELD(int, f, 0xFC); }

static inline void initStripSize(ImageStrip* strip, int size)
{
    if (!strip->m_count)
    {
        strip->m_cx = size;
        strip->m_cy = size;
    }
}

static inline void markUsed(TreeList* tree, FileTree* file)
{
    int item = fileTreeItem(file);
    fileUsed(file) = 1;
    tree->invalidateItem(item);
}

static inline bool hasDdsMagic(aStream& s)
{
    DWORD magic = *(DWORD*)(s.m_data + s.m_pos);
    return magic == 0x20534444;
}

static inline COLORREF thumbnailBackground()
{
    if (Window_isClassicAppearance())
        return GetSysColor(COLOR_BTNFACE);
    return 0xF0F0F0;
}

ClientData::ClientData(TreeList* tree, FolderRoot* root)
    : m_tree(tree), m_root(root), m_cashProducts(1024)
{
    m_loaded = false;
    for (int i = 0; i < 2; ++i)
    {
        initStripSize(&m_icons24_39[i], 32);
        initStripSize(&m_icons16_36[5 * i + 0], 32);
        initStripSize(&m_icons16_36[5 * i + 1], 32);
        initStripSize(&m_icons16_36[5 * i + 2], 32);
        initStripSize(&m_icons16_36[5 * i + 3], 32);
        initStripSize(&m_icons16_36[5 * i + 4], 32);
    }
    for (int i = 0; i < 17; ++i)
        initStripSize(&m_icons1_34[i], 32);
    initStripSize(&m_skillIcons, 32);
    initStripSize(&m_icons22, 32);
    initStripSize(&m_icons23, 32);
    initStripSize(&m_somoIcons, 32);
    initStripSize(&m_questIcons, 32);
    initStripSize(&m_quest2Icons, 32);
    initStripSize(&m_rapisIcons, 32);
    initStripSize(&m_icons40, 32);
    initStripSize(&m_somo1Icons, 32);
    initStripSize(&m_somo2Icons, 32);
    initStripSize(&m_petIcons, 32);
    initStripSize(&m_somo3Icons, 32);
    initStripSize(&m_npcFace32, 32);
    initStripSize(&m_npcFace64, 64);
}

ClientData::~ClientData()
{
}

void ClientData::clear()
{
    m_imageList.destroy();
    m_icons23.clear();
    m_icons22.clear();
    m_skillIcons.clear();
    m_somoIcons.clear();
    m_questIcons.clear();
    m_quest2Icons.clear();
    m_rapisIcons.clear();
    m_icons40.clear();
    m_somo1Icons.clear();
    m_somo2Icons.clear();
    m_somo3Icons.clear();
    m_petIcons.clear();
    m_npcFace32.clear();
    m_npcFace64.clear();
    m_items.clear();
    m_loaded = false;
}

int ClientData::addIcon(const wchar_t* folder, const wchar_t* name, ImageList* list, int id)
{
    int result;
    String path;
    path.append(folder, (unsigned)-1);
    path << L"interface\\icon\\" << name;
    bool isDds = path.m_length >= 4 && _wcsnicmp(path.m_buffer + path.m_length - 4, L".dds", 4) == 0;
    FileTree* file = (FileTree*)m_root->findFile(path.m_buffer);
    if (!file)
    {
        path.removeExtension();
        path << (isDds ? L".tga" : L".dds");
        file = (FileTree*)m_root->findFile(path.m_buffer);
        isDds = !isDds;
        if (!file)
            return 2;
    }
    ContentProvider* provider = g_contentProvider;
    if (!g_contentProvider)
        return E_FAIL;
    Bytes data;
    result = provider->readFile(file, &data, false);
    if (!result)
    {
        aStream& s = data;
        if (s.m_size >= 18)
        {
            if (isDds && !hasDdsMagic(s))
                isDds = false;
            HGDIOBJ bitmap = 0;
            if (isDds)
                result = loadDDS(data, &bitmap);
            else
                result = loadTGA(data, &bitmap);
            if (!result)
            {
                HGDIOBJ loaded = bitmap;
                HBITMAP converted = halveBitmap(loaded);
                DeleteObject(loaded);
                int size[2] = { 16, 16 };
                HBITMAP scaled = (HBITMAP)stripToRow(converted, size);
                DeleteObject(converted);
                result = E_FAIL;
                if (list->addBitmapAt(scaled, id))
                    result = 0;
                DeleteObject(scaled);
            }
        }
        else
        {
            result = E_FAIL;
        }
    }
    return result;
}

int splitIconStrip(HBITMAP bitmap, ImageStrip* strip, int firstId)
{
    if (!bitmap)
        return E_HANDLE;
    initStripSize(strip, 32);
    int size[2] = { 32, 32 };
    HBITMAP scaled = (HBITMAP)stripToRow(bitmap, size);
    DeleteObject(bitmap);
    if (!scaled)
        return E_FAIL;
    BITMAP bm;
    GetObjectW(scaled, sizeof(BITMAP), &bm);
    int count = bm.bmWidth / 32;
    for (int i = 0; i < count; ++i)
    {
        memset(&bm, 0, sizeof(bm));
        HGDIOBJ icon;
        if (GetObjectW(scaled, sizeof(BITMAP), &bm))
            icon = cropBitmap(scaled, i * bm.bmHeight, 0, bm.bmHeight, bm.bmHeight);
        else
            icon = 0;
        if (!isBlankIcon(icon, 32, 32))
            strip->add((HBITMAP)icon, i + firstId);
        DeleteObject(icon);
    }
    DeleteObject(scaled);
    return 0;
}

int ClientData::loadIconStrip(const wchar_t* folder, const wchar_t* name, ImageStrip* strip, int firstId, int )
{
    Bytes data;
    int result = 2;
    String path;
    path.append(folder, (unsigned)-1);
    path << L"interface\\icon\\" << name;
    bool isDds = path.m_length >= 4 && _wcsnicmp(&path.m_buffer[path.m_length - 4], L".dds", 4) == 0;
    FileTree* file = (FileTree*)m_root->findFile(path.m_buffer);
    if (!file)
    {
        if (firstId > 100)
            return 0;
        path.removeExtension();
        path << (isDds ? L".tga" : L".dds");
        file = (FileTree*)m_root->findFile(path.m_buffer);
        isDds = !isDds;
        if (!file)
        {
            if (!folder)
                return result;
            isDds = !isDds;
            path.removeExtension();
            path << (isDds ? L".dds" : L".tga");
            if (!PathFileExistsW(path.m_buffer))
            {
                isDds = !isDds;
                path.removeExtension();
                path << (isDds ? L".dds" : L".tga");
            }
            if (!PathFileExistsW(path.m_buffer))
                return result;
            result = data.aStream::load(path.m_buffer);
            goto decode;
        }
    }
    if (!g_contentProvider)
        return E_FAIL;
    result = g_contentProvider->readFile(file, &data, false);
decode:
    if (!result)
    {
        aStream& s = data;
        if (s.m_size >= 18)
        {
            if (isDds && !hasDdsMagic(s))
                isDds = false;
            HGDIOBJ bitmap = 0;
            if (isDds)
                result = loadDDS(data, &bitmap);
            else
                result = loadTGA(data, &bitmap);
            if (!result)
            {
                result = splitIconStrip((HBITMAP)bitmap, strip, firstId);
                if (file)
                    markUsed(m_tree, file);
            }
        }
        else
        {
            result = E_FAIL;
        }
    }
    return result;
}

int ClientData::loadSkillIcons(const wchar_t* folder, const wchar_t* name, ImageStrip* strip, int firstId)
{
    if (!g_contentProvider)
        return E_FAIL;
    String path;
    path.append(folder, (unsigned)-1);
    path << L"interface\\icon\\" << name;
    FileTree* ddsFile = (FileTree*)m_root->findFile((path + L".dds").m_buffer);
    FileTree* tgaFile = (FileTree*)m_root->findFile((path + L".tga").m_buffer);
    int result;
    if (ddsFile || tgaFile)
    {
        ImageStrip ddsIcons;
        ImageStrip tgaIcons;
        if (ddsFile)
        {
            HGDIOBJ bitmap = 0;
            Bytes data;
            aStream& s = data;
            if (!g_contentProvider->readFile(ddsFile, &data, false) && s.m_size >= 18
                && (hasDdsMagic(s) ? loadDDS(data, &bitmap) : loadTGA(data, &bitmap)) == 0)
            {
                markUsed(m_tree, ddsFile);
                splitIconStrip((HBITMAP)bitmap, &ddsIcons, firstId);
            }
        }
        if (tgaFile)
        {
            HGDIOBJ bitmap = 0;
            Bytes data;
            aStream& s = data;
            if (!g_contentProvider->readFile(tgaFile, &data, false) && s.m_size >= 18
                && !loadTGA(data, &bitmap))
            {
                markUsed(m_tree, tgaFile);
                splitIconStrip((HBITMAP)bitmap, &tgaIcons, firstId);
            }
        }
        if (ddsIcons.m_count || tgaIcons.m_count)
        {
            ImageStrip* best = &tgaIcons;
            if (tgaIcons.m_count < ddsIcons.m_count)
                best = &ddsIcons;
            strip->append(best);
            result = 0;
        }
        else
        {
            result = E_FAIL;
        }
    }
    else
    {
        result = 2;
        if (firstId > 100)
            result = 0;
    }
    return result;
}

ImageStrip* ClientData::getIcons(const wchar_t* name)
{
    String folderString = getContentFolder();
    const wchar_t* folder = folderString.m_buffer;
    if (!wcscmp(name, L"icon_skills"))
    {
        if (m_skillIcons.m_count)
            return &m_skillIcons;
        if (loadSkillIcons(folder, L"icon_skill", &m_skillIcons, 1)
            || loadSkillIcons(folder, L"icon_skill2", &m_skillIcons, 1001))
            return 0;
        if (!loadSkillIcons(folder, L"icon_skill3", &m_skillIcons, 2001))
            return &m_skillIcons;
        return 0;
    }
    if (!wcscmp(name, L"icon_rapis"))
    {
        if (m_rapisIcons.m_count || !loadIconStrip(folder, L"icon_rapis.dds", &m_rapisIcons, 1, 0))
            return &m_rapisIcons;
        return 0;
    }
    if (!wcscmp(name, L"icon_somo1"))
    {
        if (m_somo1Icons.m_count || !loadIconStrip(folder, L"icon_somo1.dds", &m_somo1Icons, 1, 0))
            return &m_somo1Icons;
        return 0;
    }
    if (!wcscmp(name, L"icon_somo2"))
    {
        if (m_somo2Icons.m_count || !loadIconStrip(folder, L"icon_somo2.dds", &m_somo2Icons, 1, 0))
            return &m_somo2Icons;
        return 0;
    }
    if (!wcscmp(name, L"icon_somo3"))
    {
        if (m_somo3Icons.m_count || !loadIconStrip(folder, L"icon_somo3.dds", &m_somo3Icons, 1, 0))
            return &m_somo3Icons;
        return 0;
    }
    if (!wcscmp(name, L"icon_pet"))
    {
        if (m_petIcons.m_count || !loadIconStrip(folder, L"icon_pet.dds", &m_petIcons, 1, 0))
            return &m_petIcons;
        return 0;
    }
    if (wcscmp(name, L"npc_face32") && wcscmp(name, L"npc_face64"))
        return 0;
    if (!m_npcFace32.m_count)
    {
        FolderTree* iface = (FolderTree*)m_root->FolderTree::findChildFolder(L"interface");
        if (iface)
        {
            FolderTree* faces = (FolderTree*)iface->FolderTree::findChildFolder(L"NpcFace");
            if (faces)
            {
                BitmapsID ids;
                if (loadBitmapsID(faces, &ids) == 0)
                {
                    int count = ids.m_count;
                    for (int i = 0; i < count; ++i)
                    {
                        long id = ids.keyAt(i);
                        HBITMAP bitmap = *ids.valueAt(i);
                        if (bitmap)
                        {
                            m_npcFace64.add(bitmap, id);
                            HBITMAP scaled = halveBitmap(bitmap);
                            m_npcFace32.add(scaled, id);
                            DeleteObject(scaled);
                        }
                    }
                }
            }
        }
        if (!m_npcFace32.m_count)
            return 0;
    }
    if (wcscmp(name, L"npc_face32"))
        return &m_npcFace64;
    return &m_npcFace32;
}

ImageStrip* ClientData::getItemIcons(int type)
{
    String folderString = getContentFolder();
    const wchar_t* folder = folderString.m_buffer;
    ImageStrip* strip;
    int number;
    switch (type)
    {
    case 30: case 98: case 95:
        if (!m_rapisIcons.m_count)
            loadIconStrip(folder, L"icon_rapis.dds", &m_rapisIcons, 1, 0);
        return &m_rapisIcons;
    case 25: case 41: case 38: case 42: case 43: case 44: case 94:
        if (!m_somoIcons.m_count)
            loadIconStrip(folder, L"icon_somo.dds", &m_somoIcons, 1, 0);
        return &m_somoIcons;
    case 78: case 79: case 80:
        if (!m_somo1Icons.m_count)
            loadIconStrip(folder, L"icon_somo1.dds", &m_somo1Icons, 1, 0);
        return &m_somo1Icons;
    case 27: case 28: case 29:
        if (!m_questIcons.m_count)
            loadIconStrip(folder, L"icon_quest.dds", &m_questIcons, 1, 0);
        return &m_questIcons;
    case 99:
        if (!m_quest2Icons.m_count)
            loadIconStrip(folder, L"icon_quest2.dds", &m_quest2Icons, 1, 0);
        return &m_quest2Icons;
    case 100: case 101:
        if (!m_somo2Icons.m_count)
            loadIconStrip(folder, L"icon_somo2.dds", &m_somo2Icons, 1, 0);
        return &m_somo2Icons;
    case 102: case 103:
        if (!m_somo3Icons.m_count)
            loadIconStrip(folder, L"icon_somo3.dds", &m_somo3Icons, 1, 0);
        return &m_somo3Icons;
    case 120:
        if (!m_petIcons.m_count)
            loadIconStrip(folder, L"icon_pet.dds", &m_petIcons, 1, 0);
        return &m_petIcons;
    case 121:
        if (!m_wingIcons.m_count)
            loadIconStrip(folder, L"icon_Wing.dds", &m_wingIcons, 1, 0);
        return &m_wingIcons;
    case 150:
        if (!m_dualLayerIcons.m_count)
            loadIconStrip(folder, L"icon_DualLayer.dds", &m_dualLayerIcons, 1, 0);
        return &m_dualLayerIcons;
    case 1:  case 45:           strip = &m_icons1_34[0];   number = 1;  break;
    case 2:  case 46:           strip = &m_icons1_34[1];   number = 2;  break;
    case 3:  case 47:           strip = &m_icons1_34[2];   number = 3;  break;
    case 4:  case 48:           strip = &m_icons1_34[3];   number = 4;  break;
    case 5:  case 49: case 50:  strip = &m_icons1_34[4];   number = 5;  break;
    case 6:  case 51: case 52:  strip = &m_icons1_34[5];   number = 6;  break;
    case 7:  case 53: case 54:  strip = &m_icons1_34[6];   number = 7;  break;
    case 8:  case 55: case 56:  strip = &m_icons1_34[7];   number = 8;  break;
    case 9:  case 57:           strip = &m_icons1_34[8];   number = 9;  break;
    case 10: case 58:           strip = &m_icons1_34[9];   number = 10; break;
    case 11: case 59:           strip = &m_icons1_34[10];  number = 11; break;
    case 12: case 60: case 61:  strip = &m_icons1_34[11];  number = 12; break;
    case 13: case 62: case 63:  strip = &m_icons1_34[12];  number = 13; break;
    case 14: case 64:           strip = &m_icons1_34[13];  number = 14; break;
    case 15: case 65:           strip = &m_icons1_34[14];  number = 15; break;
    case 16: case 66: case 72:  strip = &m_icons16_36[0];  number = 16; break;
    case 17: case 67: case 73:  strip = &m_icons16_36[1];  number = 17; break;
    case 18: case 68: case 74:  strip = &m_icons16_36[2];  number = 18; break;
    case 19: case 69:           strip = &m_icons1_34[15];  number = 19; break;
    case 20: case 70: case 76:  strip = &m_icons16_36[3];  number = 20; break;
    case 21: case 71: case 77:  strip = &m_icons16_36[4];  number = 21; break;
    case 22: case 37:           strip = &m_icons22;        number = 22; break;
    case 23: case 96:           strip = &m_icons23;        number = 23; break;
    case 24:                    strip = &m_icons24_39[0];  number = 24; break;
    case 31: case 81: case 87:  strip = &m_icons16_36[5];  number = 31; break;
    case 32: case 82: case 88:  strip = &m_icons16_36[6];  number = 32; break;
    case 33: case 83: case 89:  strip = &m_icons16_36[7];  number = 33; break;
    case 34: case 84:           strip = &m_icons1_34[16];  number = 34; break;
    case 35: case 85: case 91:  strip = &m_icons16_36[8];  number = 35; break;
    case 36: case 86: case 92:  strip = &m_icons16_36[9];  number = 36; break;
    case 39:                    strip = &m_icons24_39[1];  number = 39; break;
    case 40: case 97:           strip = &m_icons40;        number = 40; break;
    default:
        return 0;
    }
    if (!strip->m_count)
    {
        String fileName;
        if (number < 10)
            fileName << '0';
        fileName << (long)number << ".dds";
        if (loadIconStrip(folder, fileName.m_buffer, strip, 1, 0))
        {
            fileName.clear();
            fileName << (long)(number + 100) << ".dds";
            loadIconStrip(folder, fileName.m_buffer, strip, 101, 0);
        }
    }
    return strip;
}

ImageList* ClientData::getImageList()
{
    ImageList* list = &m_imageList;
    if (!m_imageList.m_hList || !ImageList_GetImageCount(m_imageList.m_hList))
    {
        list->m_cx = 16;
        list->m_cy = 16;
        list->m_hList = ImageList_Create(16, 16, ILC_COLOR32, 64, 128);
        wchar_t buffer[16] = { 0 };
        String folderString = getContentFolder();
        const wchar_t* folder = folderString.m_buffer;
        unsigned number = 1;
        for (int id = 1101; id <= 15101; id += 1000)
        {
            swprintf_s(buffer, 16, L"%0.2u.dds", number);
            addIcon(folder, buffer, list, id - 100);
            swprintf_s(buffer, 16, L"%0.2u.dds", number + 100);
            addIcon(folder, buffer, list, id);
            ++number;
        }
        number = 16;
        for (int id = 16001; id <= 24001; id += 1000)
        {
            swprintf_s(buffer, 16, L"%0.2u.dds", number);
            addIcon(folder, buffer, list, id);
            ++number;
        }
        addIcon(folder, L"icon_somo.dds", list, 25001);
        addIcon(folder, L"icon_quest.dds", list, 27001);
        addIcon(folder, L"icon_rapis.dds", list, 30001);
        number = 31;
        for (int id = 31001; id <= 40001; id += 1000)
        {
            swprintf_s(buffer, 16, L"%0.2u.dds", number);
            addIcon(folder, buffer, list, id);
            ++number;
        }
        addIcon(folder, L"icon_somo1.dds", list, 78001);
        addIcon(folder, L"icon_quest2.dds", list, 99001);
        addIcon(folder, L"icon_somo2.dds", list, 100001);
        addIcon(folder, L"icon_somo3.dds", list, 102001);
        addIcon(folder, L"icon_pet.dds", list, 120001);
        addIcon(folder, L"icon_Wing.dds", list, 121001);
        addIcon(folder, L"icon_DualLayer.dds", list, 150001);
    }
    return list;
}

int itemIconBase(unsigned char type)
{
    if (type == 25 || type == 38 || type == 41 || type == 42 || type == 43 || type == 44 || type == 94)
        return 25000;
    if ((unsigned char)(type - 27) <= 2u)
        return 27000;
    if ((unsigned char)(type - 78) <= 2u)
        return 78000;
    if (type == 100 || type == 101)
        return 100000;
    if (type == 120 || type == 150)
        return 1000 * type;
    return 1000 * (unsigned char)itemIconSheet((char)type);
}

char itemIconSheet(char type)
{
    char result;
    switch (type)
    {
    case 1:  case 45:           result = 1;  break;
    case 2:  case 46:           result = 2;  break;
    case 3:  case 47:           result = 3;  break;
    case 4:  case 48:           result = 4;  break;
    case 5:  case 49: case 50:  result = 5;  break;
    case 6:  case 51: case 52:  result = 6;  break;
    case 7:  case 53: case 54:  result = 7;  break;
    case 8:  case 55: case 56:  result = 8;  break;
    case 9:  case 57:           result = 9;  break;
    case 10: case 58:           result = 10; break;
    case 11: case 59:           result = 11; break;
    case 12: case 60: case 61:  result = 12; break;
    case 13: case 62: case 63:  result = 13; break;
    case 14: case 64:           result = 14; break;
    case 15: case 65:           result = 15; break;
    case 16: case 66: case 72:  result = 16; break;
    case 17: case 67: case 73:  result = 17; break;
    case 18: case 68: case 74:  result = 18; break;
    case 19: case 69:           result = 19; break;
    case 20: case 70: case 76:  result = 20; break;
    case 21: case 71: case 77:  result = 21; break;
    case 22: case 37:           result = 22; break;
    case 23: case 96:           result = 23; break;
    case 24:                    result = 24; break;
    case 25:                    result = 25; break;
    case 30: case 95: case 98:  result = 30; break;
    case 31: case 81: case 87:  result = 31; break;
    case 32: case 82: case 88:  result = 32; break;
    case 33: case 83: case 89:  result = 33; break;
    case 34: case 84:           result = 34; break;
    case 35: case 85: case 91:  result = 35; break;
    case 36: case 86: case 92:  result = 36; break;
    case 39:                    result = 39; break;
    case 40: case 97:           result = 40; break;
    default:                    result = type; break;
    }
    return result;
}

Model3D* ClientData::loadModel(const String& model, const String& texture, DWORD tag)
{
    ContentProvider* provider = g_contentProvider;
    if (!g_contentProvider)
        return 0;
    Bytes textureData;
    if (provider->readPath(texture.m_buffer, &textureData))
        return 0;
    Bytes modelData;
    HGDIOBJ bitmap;
    if (g_contentProvider->readPath(model.m_buffer, &modelData)
        || (bitmap = 0, loadDDS(textureData, &bitmap)))
        return 0;
    Model3D* result = new Model3D(tag);
    if (!result)
    {
        DeleteObject(bitmap);
        return 0;
    }
    result->read(modelData, 'file');
    result->setBitmap((HBITMAP)bitmap);
    return result;
}

MonsterModels* ClientData::getMonsterModels(const wchar_t* name)
{
    void* file = 0;
    if (!wcscmp(name, L"npc"))
    {
        FolderTree* folder = (FolderTree*)m_root->FolderTree::findChildFolder(L"NPC");
        if (folder)
            file = folder->FolderTree::findChildFile(L"npc.mon");
    }
    if (!wcscmp(name, L"monsters"))
    {
        FolderTree* folder = (FolderTree*)m_root->FolderTree::findChildFolder(L"Monster");
        if (folder)
            file = folder->FolderTree::findChildFile(L"monster.mon");
    }
    if (file)
        return m_monsterModels.load((FileTree*)file, m_tree);
    return 0;
}

ModelsMLT* ClientData::getCharModels(int classFlags, int itemType, bool male, String* folderOut)
{
    String folder = getContentFolder();
    String path;
    path.append(L"Character\\", (unsigned)-1);
    if (itemType == 121)
    {
        path << L"Wing";
    }
    else
    {
        const wchar_t* raceFolder;
        if (classFlags & 1)
            raceFolder = L"Human\\";
        else if (classFlags & 2)
            raceFolder = L"Human\\";
        else if (classFlags & 4)
            raceFolder = L"Elf\\";
        else if (classFlags & 8)
            raceFolder = L"Elf\\";
        else if (classFlags & 0x20)
            raceFolder = L"Elf\\";
        else if (classFlags & 0x10)
            raceFolder = L"Human\\";
        else if (classFlags & 0x100)
            raceFolder = L"DeathEater\\";
        else if (classFlags & 0x200)
            raceFolder = L"DeathEater\\";
        else if (classFlags & 0x400)
            raceFolder = L"Vile\\";
        else if (classFlags & 0x800)
            raceFolder = L"DeathEater\\";
        else if ((classFlags & 0x2000) || (classFlags & 0x1000))
            raceFolder = L"Vile\\";
        else
            return 0;
        path << raceFolder;
    }
    int race = Data::Item::classFromFlags((short)classFlags);
    if (race == -2)
        return 0;
    int part = Data::Item::equipSlot(itemType);
    if (part == -1)
        return 0;
    ModelsMLT* models = &m_charModels[race][part][male == 0];
    if (models->isEmpty())
    {
        wchar_t gender = male ? L'm' : L'w';
        int kind = 'm';
        const wchar_t* prefix;
        switch (race)
        {
        case 0:
        case 1:
            if (race == 0)
                kind = 'f';
            prefix = L"hu";
            break;
        case 2:
        case 3:
            if (race == 2)
                kind = 'r';
            prefix = L"el";
            break;
        case 4:
        case 5:
            kind = 'r';
            if (race == 4)
                kind = 'f';
            prefix = L"de";
            break;
        case 6:
        case 7:
            if (race == 6)
                kind = 'r';
            prefix = L"vi";
            break;
        default:
            return 0;
        }
        path << prefix << gender << (char)kind;
        switch (part)
        {
        case 0: path << L"_face"; break;
        case 1: path << L"_foot"; break;
        case 2: path << L"_hair"; break;
        case 3: path << L"_hand"; break;
        case 4: path << L"_helmet"; break;
        case 5: path << L"_lower"; break;
        case 6: path << L"_upper"; break;
        default: break;
        }
        path << L".mlt";
        iStream in;
        bool failed;
        {
            String fullPath = folder + path;
            failed = studioLoadFile(&in, &fullPath) != 0;
        }
        if (failed)
            return 0;
        models->read(in, 'file');
        FileTree* file = (FileTree*)m_root->findFile(path.m_buffer);
        if (file)
            markUsed(m_tree, file);
    }
    if (models->isEmpty())
        return 0;
    if (folderOut)
        *folderOut = path;
    return models;
}

ItemModels* ClientData::getItemModels(char type, String* pathOut)
{
    String folder = getContentFolder();
    String path;
    path.append(L"Item\\", (unsigned)-1);
    ItemModels* models;
    int number;
    switch (type)
    {
    case 1:  case 45:           models = &m_itemModels[0];  number = 1;  break;
    case 2:  case 46:           models = &m_itemModels[1];  number = 2;  break;
    case 3:  case 47:           models = &m_itemModels[2];  number = 3;  break;
    case 4:  case 48:           models = &m_itemModels[3];  number = 4;  break;
    case 5:  case 49: case 50:  models = &m_itemModels[4];  number = 5;  break;
    case 6:  case 51: case 52:  models = &m_itemModels[5];  number = 6;  break;
    case 7:  case 53: case 54:  models = &m_itemModels[6];  number = 7;  break;
    case 8:  case 55: case 56:  models = &m_itemModels[7];  number = 8;  break;
    case 9:  case 57:           models = &m_itemModels[8];  number = 9;  break;
    case 10: case 58:           models = &m_itemModels[9];  number = 10; break;
    case 11: case 59:           models = &m_itemModels[10]; number = 11; break;
    case 12: case 60: case 61:  models = &m_itemModels[11]; number = 12; break;
    case 13: case 62: case 63:  models = &m_itemModels[12]; number = 13; break;
    case 14: case 64:           models = &m_itemModels[13]; number = 14; break;
    case 15: case 65:           models = &m_itemModels[14]; number = 15; break;
    case 19: case 69:           models = &m_itemModels[15]; number = 19; break;
    case 34: case 84:           models = &m_itemModels[16]; number = 34; break;
    default:
        return 0;
    }
    if (models->isEmpty())
    {
        if (number < 10)
            path << L'0';
        path << (long)number << ".itm";
        iStream in;
        bool failed;
        {
            String fullPath = folder + path;
            failed = studioLoadFile(&in, &fullPath) != 0;
        }
        if (failed)
            return 0;
        models->read(in, 'file');
        FileTree* file = (FileTree*)m_root->findFile(path.m_buffer);
        if (file)
            markUsed(m_tree, file);
    }
    if (models->isEmpty())
        return 0;
    if (pathOut)
        *pathOut = path;
    return models;
}

MonsterModels* ClientData::getWingModels(int )
{
    String folder = getContentFolder();
    String path;
    path.append(L"Character\\Wing\\Wing.mon", (unsigned)-1);
    iStream in;
    bool failed;
    {
        String fullPath = folder + path;
        failed = studioLoadFile(&in, &fullPath) != 0;
    }
    if (failed)
        return 0;
    m_wingModels.read(in, 'file');
    FileTree* file = (FileTree*)m_root->findFile(path.m_buffer);
    if (file)
        markUsed(m_tree, file);
    if (m_wingModels.ptrArray<MonsterModel*>::isEmpty())
        return 0;
    return &m_wingModels;
}

static String charFolderFromName(const String& name, int* race)
{
    String result;
    result.append(L"Character\\", (unsigned)-1);
    wchar_t kind;
    if (name.m_buffer && name.m_length > 3u)
        kind = name.m_buffer[3];
    else
        kind = 0;
    *race = -2;
    if (name.m_length >= 2u && !wcsncmp(name.m_buffer, L"hu", 2))
    {
        *race = kind != 'f';
        result << L"Human\\";
        return result;
    }
    if (name.m_length >= 2u && !wcsncmp(name.m_buffer, L"el", 2))
    {
        *race = (kind == 'm') + 2;
        result << L"Elf\\";
        return result;
    }
    if (name.startsWith(L"de"))
    {
        *race = (kind != 'f') + 4;
        result << "DeathEater\\";
    }
    else if (name.startsWith(L"vi"))
    {
        *race = (kind == 'm') + 6;
        result << "Vile\\";
    }
    else if (name.startsWith(L"tyro_type2_"))
    {
        *race = 11;
        result << "tyro_type2\\";
    }
    else if (name.startsWith(L"tyro_"))
    {
        *race = 10;
        result << "Tyross\\";
    }
    else if (name.startsWith(L"vl"))
    {
        *race = 8;
        result << "Vueltas\\";
    }
    return result;
}

ModelsMLT* ClientData::getCharModelsByName(const String& fileName)
{
    int race = -2;
    String path = charFolderFromName(fileName, &race);
    if (race == -2)
        return 0;
    path.append(fileName.m_buffer, fileName.m_length);
    int gender;
    if (fileName.m_buffer && fileName.m_length > 2u)
        gender = (unsigned short)fileName.m_buffer[2];
    else
        gender = 0;
    ModelsMLT* table;
    int index;
    if (race != 10 && race != 11)
    {
        int part;
        if (fileName.containsNoCase(L"face"))
            part = 0;
        else if (fileName.containsNoCase(L"foot"))
            part = 1;
        else if (fileName.containsNoCase(L"hair"))
            part = 2;
        else if (fileName.containsNoCase(L"hand"))
            part = 3;
        else if (fileName.containsNoCase(L"helmet"))
            part = 4;
        else if (fileName.containsNoCase(L"lower"))
            part = 5;
        else if (fileName.containsNoCase(L"upper"))
            part = 6;
        else
            return 0;
        table = &m_charModels[0][0][0];
        index = ((WORD)gender == 'w') + 2 * (part + 7 * race);
    }
    else
    {
        if (fileName.containsNoCase(L"back"))
            index = 0;
        else if (fileName.containsNoCase(L"helmet"))
            index = 5;
        else if (fileName.containsNoCase(L"l_hand"))
            index = 3;
        else if (fileName.containsNoCase(L"l_shield"))
            index = 1;
        else if (fileName.containsNoCase(L"lower"))
            index = 6;
        else if (fileName.containsNoCase(L"missile"))
            index = 2;
        else if (fileName.containsNoCase(L"r_hand"))
            index = 4;
        else if (fileName.containsNoCase(L"upper"))
            index = 7;
        else
            return 0;
        if (race == 10)
            table = &m_tyrossModels[0][0];
        else
            table = &m_tyrossModels[1][0];
    }
    ModelsMLT* models = &table[index];
    if (models->isEmpty())
    {
        iStream in;
        bool failed;
        {
            String fullPath = getContentFolder() + path;
            failed = studioLoadFile(&in, &fullPath) != 0;
        }
        if (failed)
            return 0;
        models->read(in, 'file');
        FileTree* file = (FileTree*)m_root->findFile(path.m_buffer);
        if (file)
            markUsed(m_tree, file);
    }
    if (models->isEmpty())
        return 0;
    return models;
}

ModelsMLX* ClientData::getMLXByName(const String& fileName)
{
    int race = -2;
    String path = charFolderFromName(fileName, &race);
    if (race == -2)
        return 0;
    path.append(fileName.m_buffer, fileName.m_length);
    wchar_t gender;
    if (fileName.m_buffer && fileName.m_length > 2u)
        gender = fileName.m_buffer[2];
    else
        gender = 0;
    ModelsMLX* models = &m_charMLX[0][0] + 2 * race + (gender == 'w');
    if (!models->isEmpty())
        return models;
    iStream in;
    bool failed;
    {
        String fullPath = getContentFolder() + path;
        failed = studioLoadFile(&in, &fullPath) != 0;
    }
    if (failed)
        return 0;
    models->read(in, 'file');
    FileTree* file = (FileTree*)m_root->findFile(path.m_buffer);
    if (file)
        markUsed(m_tree, file);
    return models;
}

int ClientData::renderCharItem(int classFlags, int itemType, char male, int modelId, int , int cx, int cy)
{
    String folder;
    ModelsMLT* models = getCharModels(classFlags, itemType, male != 0, &folder);
    if (!models || models->m_count <= 0)
        return 0;
    ModelMLT** items = models->m_items;
    int i = 0;
    ModelMLT* model;
    while (1)
    {
        model = *items;
        if (FIELD(int, model, 0x0C) == modelId)
            break;
        ++i;
        ++items;
        if (i >= models->m_count)
            return 0;
    }
    folder.removeFileName();
    String modelPath(folder);
    String texturePath(folder);
    modelPath << "3DC\\" << model->getString('vert');
    texturePath << "DDS\\" << model->getString('text');
    int result;
    Model3D* m3d = loadModel(modelPath, texturePath, 'k3DC');
    if (m3d)
    {
        COLORREF background = thumbnailBackground();
        SIZE size = { cx, cy };
        result = (int)(INT_PTR)m3d->makeBitmap(background, size, 0, 'k3DC');
        delete m3d;
    }
    else
    {
        result = 0;
    }
    return result;
}

int ClientData::renderWing(int index, int , int cx, int cy)
{
    MonsterModels* wings = getWingModels((int)(INT_PTR)this);
    if (!wings)
        return 0;
    if (index < 0)
        return 0;
    if (index >= wings->ptrArray<MonsterModel*>::m_count)
        return 0;
    MonsterModel* wing = wings->ptrArray<MonsterModel*>::m_items[index];
    if (!wing)
        return 0;
    String base = getContentFolder() + L"Character\\Wing\\";
    String modelPath(base);
    String texturePath(base);
    modelPath << "3DC\\" << wing->getString('vert');
    texturePath << "DDS\\" << wing->getString('text');
    int result;
    Model3D* m3d = loadModel(modelPath, texturePath, 'k3DC');
    if (m3d)
    {
        COLORREF background = thumbnailBackground();
        SIZE size = { cx, cy };
        result = (int)(INT_PTR)m3d->makeBitmap(background, size, 0, 'wing');
        delete m3d;
    }
    else
    {
        result = 0;
    }
    return result;
}

int ClientData::renderItem(char type, int index, int background, int cx, int cy)
{
    String path;
    ItemModels* models = getItemModels(type, &path);
    ItemModel* item;
    if (!models || (item = models->get(index)) == 0)
        return 0;
    String modelPath(path);
    String texturePath(path);
    modelPath << "3DO\\" << item->getString('vert');
    texturePath << "DDS\\" << item->getString('text');
    int result;
    Model3D* m3d = loadModel(modelPath, texturePath, 'k3DO');
    if (m3d)
    {
        DWORD kind = 0;
        switch (type)
        {
        case 19: case 34: case 69: case 84:
            kind = 5;
            break;
        case 14: case 64:
            kind = 7;
            break;
        case 15: case 65:
            kind = 8;
            break;
        case 13: case 62: case 63:
            kind = 10;
            break;
        }
        if (!FIELD(int, item, 0x28))
        {
            char sheet = Data::Item::baseType(type);
            if (sheet == 11 || sheet == 13 || sheet == 15)
                kind |= 0x10000;
        }
        SIZE size = { cx, cy };
        result = (int)(INT_PTR)m3d->makeBitmap((COLORREF)background, size, FIELD(int, item, 0x30), kind);
        delete m3d;
    }
    else
    {
        result = 0;
    }
    return result;
}

int ClientData::renderCape(const wchar_t* model, const wchar_t* texture)
{
    String folder = getContentFolder();
    Model3D* m3d;
    {
        String texturePath = folder + texture;
        String modelPath = folder + model;
        m3d = loadModel(modelPath, texturePath, 'k3DC');
    }
    int result;
    if (m3d)
    {
        m3d->setFlag(1, true);
        COLORREF background = thumbnailBackground();
        SIZE size = { 256, 512 };
        result = (int)(INT_PTR)m3d->makeBitmap(background, size, 0, 'cape');
        delete m3d;
    }
    else
    {
        result = 0;
    }
    return result;
}

bool ClientData::loadSData(Serializable* data, const wchar_t* path, DWORD fmt, int decrypt)
{
    if (!data->isEmpty())
        return true;
    FileTree* file = (FileTree*)m_root->findFile(path);
    if (file && g_contentProvider)
    {
        iStream in;
        if (!studioLoadStream(&in, file, decrypt))
        {
            data->read(in, fmt);
            if (fmt == 'flnt')
            {
                UINT language = g_preferences.getInt(L"SData", L"language", 0);
                if (!language)
                {
                    wchar_t* fileName = PathFindFileNameW(path);
                    String message(L"You are loading a binary-only file ", (unsigned)-1);
                    if (fileName)
                        message << '(' << fileName << ") ";
                    message << L"and no prefered language was defined.\nUse the settings dialog to define your language.";
                    AlertDial::alert(0, message.m_buffer, 0, 16);
                    throw 1;
                }
                String languageFile = getContentFolder();
                languageFile << data->getFilePrefix();
                const wchar_t* suffix;
                if (languageFile.containsNoCase(L"BinarySData"))
                    suffix = countryCode(language);
                else
                    suffix = countryName(language);
                languageFile << suffix << L".SData";
                Bytes text;
                typedef int (__thiscall* LoadFn)(void*, Bytes*, String*, int);
                if ((*(LoadFn*)(*(char**)((void*)g_studioFrame) + 332))(((void*)g_studioFrame), &text, &languageFile, 0))
                    throw 1;
                data->load(text, 0, (int)language);
            }
            markUsed(m_tree, file);
            return true;
        }
    }
    return false;
}

bool ClientData::loadSData(Serializable* data, const wchar_t* pathText, const wchar_t* pathBinary, int )
{
    bool binary;
    {
        String primary = g_preferences.getString(L"SData", L"Primary", L"undefined");
        binary = primary.m_length == 7 && _wcsnicmp(primary.m_buffer, L"DBfiles", 7) == 0;
    }
    if (binary)
        return loadSData(data, pathBinary, 'flnt', 1);
    return loadSData(data, pathText, 'file', 1);
}

bool ClientData::loadMonsters()
{
    Monsters* monsters = &m_monsters;
    if (!m_monsters.isEmpty())
        return true;
    bool binary;
    {
        String primary = g_preferences.getString(L"SData", L"Primary", L"undefined");
        binary = primary.equalsNoCase(L"DBfiles");
    }
    Serializable* data = monsters ? static_cast<Serializable*>(monsters) : 0;
    if (binary)
        return loadSData(data, L"BinarySData\\DBMonsterData.sdata", 'flnt', 1);
    FIELD(bool, monsters, 0x30) = true;
    bool result = loadSData(data, L"Monster\\monsterEx.sdata", 'file', 1);
    if (!result)
    {
        FIELD(bool, monsters, 0x30) = false;
        return loadSData(data, L"Monster\\monster.sdata", 'file', 1);
    }
    return result;
}
