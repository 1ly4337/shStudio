#pragma once
#include <windows.h>
#include "String.hpp"
#include "vLong.h"
#include "ImageList.h"
#include "ImageStrip.h"
#include "CashProducts.h"
#include "Data_Items.h"
#include "Data_ItemSets.h"
#include "Monsters.h"
#include "MobsSkills.h"
#include "ToonSkills.h"
#include "NpcData.h"
#include "ModelsMLT.h"
#include "ItemModels.h"
#include "ClothTextures.h"
#include "ModelsMLX.h"
#include "MonsterModels.h"
#include "CharActions.h"
#include "ModelFiles.h"
#include "TacticsZones.h"

class TreeList;
class FolderRoot;
class FileTree;
class Serializable;
class Model3D;

class ClientData
{
public:
    ClientData(TreeList* tree, FolderRoot* root);
    ~ClientData();

    void clear();

    int addIcon(const wchar_t* folder, const wchar_t* name, ImageList* list, int id);
    int loadIconStrip(const wchar_t* folder, const wchar_t* name, ImageStrip* strip, int firstId, int unused);
    int loadSkillIcons(const wchar_t* folder, const wchar_t* name, ImageStrip* strip, int firstId);
    ImageStrip* getIcons(const wchar_t* name);
    ImageStrip* getItemIcons(int type);
    ImageList* getImageList();

    static Model3D* loadModel(const String& model, const String& texture, DWORD tag);
    MonsterModels* getMonsterModels(const wchar_t* name);
    ModelsMLT* getCharModels(int classFlags, int itemType, bool male, String* folderOut);
    ItemModels* getItemModels(char type, String* pathOut);
    MonsterModels* getWingModels(int unused);
    ModelsMLT* getCharModelsByName(const String& fileName);
    ModelsMLX* getMLXByName(const String& fileName);
    int renderCharItem(int classFlags, int itemType, char male, int modelId, int a6, int cx, int cy);
    int renderWing(int index, int a3, int cx, int cy);
    int renderItem(char type, int index, int a4, int cx, int cy);
    static int renderCape(const wchar_t* model, const wchar_t* texture);

    bool loadSData(Serializable* data, const wchar_t* path, DWORD fmt, int decrypt);
    bool loadSData(Serializable* data, const wchar_t* pathText, const wchar_t* pathBinary, int unused);
    bool loadMonsters();

    TreeList*        m_tree;
    FolderRoot*      m_root;
    bool             m_loaded;
    ImageList        m_imageList;
    ImageStrip       m_skillIcons;
    ImageStrip       m_icons22;
    ImageStrip       m_icons23;
    ImageStrip       m_icons24_39[2];
    ImageStrip       m_somoIcons;
    ImageStrip       m_questIcons;
    ImageStrip       m_quest2Icons;
    ImageStrip       m_rapisIcons;
    ImageStrip       m_icons40;
    ImageStrip       m_somo1Icons;
    ImageStrip       m_somo2Icons;
    ImageStrip       m_somo3Icons;
    ImageStrip       m_petIcons;
    ImageStrip       m_wingIcons;
    ImageStrip       m_dualLayerIcons;
    ImageStrip       m_icons16_36[10];
    ImageStrip       m_icons1_34[17];
    ImageStrip       m_npcFace32;
    ImageStrip       m_npcFace64;
    CashProducts     m_cashProducts;
    Data::Items      m_items;
    Data::ItemSets   m_itemSets;
    Monsters         m_monsters;
    MobsSkills       m_mobsSkills;
    ToonSkills       m_toonSkills;
    NpcData          m_quests;
    ModelsMLT        m_charModels[9][7][2];
    ModelsMLT        m_tyrossModels[2][8];
    ItemModels       m_itemModels[17];
    ClothTextures    m_clothTextures[4];
    ModelsMLX        m_charMLX[9][2];
    MonsterModels    m_wingModels;
    ModelFiles<CharActions>   m_charActions;
    ModelFiles<MonsterModels> m_monsterModels;
    TacticsZones     m_tacticsZones;
};
static_assert(sizeof(ClientData) == 0x3300, "ClientData size");

int splitIconStrip(HBITMAP bitmap, ImageStrip* strip, int firstId);
int itemIconBase(unsigned char type);
char itemIconSheet(char type);
