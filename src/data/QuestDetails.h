#pragma once
#include <windows.h>
#include <cstdint>
#include "Node.h"
#include "String.hpp"
#include "Bytes.h"
#include "iStream.h"
#include "oStream.h"

#pragma pack(push, 1)
struct QuestReq {
    int16_t minLevel;
    int16_t maxLevel;
    uint8_t country;
    uint8_t grow;
    uint8_t na2;
    uint8_t na3;
    uint8_t classes[6];
    int16_t prv2;
    int16_t prv3;
    int16_t na4;
    uint8_t na5;
    uint8_t _pad15;
    int16_t prv1;
    int     na6;
    int     u1C;
    int16_t u20;
    uint8_t u22;
    uint8_t _pad23;
    int16_t timer;
    int16_t _pad26;
    int     u28;
    int     u2C;
    int16_t u30;
    uint8_t rept;
    uint8_t u33;
    int16_t u34;
    uint8_t qmod;
    uint8_t nty1;
    int16_t nti1;
    uint8_t ity0;
    uint8_t iti0;
    uint8_t rty1;
    uint8_t rti1;
    uint8_t rcat1;
    uint8_t rty2;
    uint8_t rti2;
    uint8_t rcat2;
    uint8_t rty3;
    uint8_t rti3;
    uint8_t rcat3;
    uint8_t qtyp;
    uint8_t nty2;
    uint8_t _pad47;
    int16_t nti2;
    uint8_t fty1;
    uint8_t fti1;
    uint8_t fct1;
    uint8_t fty2;
    uint8_t fti2;
    uint8_t fct2;
    uint8_t fty3;
    uint8_t fti3;
    uint8_t fct3;
    uint8_t kpvp;
    int16_t mbi1;
    uint8_t mct1;
    uint8_t _pad57;
    int16_t mbi2;
    uint8_t mct2;
    uint8_t drwd;
    uint8_t clrw;
    uint8_t _pad5D[3];

    void read(iStream& s);
    oStream& write(oStream& s) const;
    bool toText(String& str, int format) const;
};
static_assert(sizeof(QuestReq) == 0x60, "QuestReq size");

struct QuestResult {
    uint8_t mk;
    uint8_t mu;
    uint8_t mc;
    uint8_t ik;
    uint8_t ic;
    uint8_t iu;
    uint8_t _pad06[2];
    int     u1;
    int     u2;
    int     exp;
    int     gold;
    uint8_t t[3];
    uint8_t i[3];
    uint8_t c[3];
    uint8_t _pad21;
    int16_t unl;

    void clear();
    void read(iStream& s, int16_t version);
    oStream& write(oStream& s, int16_t version) const;
};
static_assert(sizeof(QuestResult) == 0x24, "QuestResult size");
#pragma pack(pop)

class QuestDetails : public Node {
public:
    explicit QuestDetails(int version);
    virtual ~QuestDetails();

    virtual const wchar_t* getString(DWORD tag) const;
    virtual int  getInt(DWORD tag) const;
    virtual bool setInt(DWORD tag, int value);
    virtual bool setString(DWORD tag, const wchar_t* text);
    virtual void reset();
    virtual int  compare(const Node* other, DWORD tag) const;

    virtual void read(iStream& s, DWORD tag);
    virtual void write(oStream& s, DWORD tag) const;
    virtual bool toText(String& out, DWORD format) const;

    void cleanStrings();
    void readRawString(iStream& s, String& dst);

public:
    int         m_version;
    int16_t     m_questId;
    int16_t     _pad12;
    String      m_name;
    String      m_summary;
    String      m_replies[6];
    String      m_desc;
    String      m_welcome;
    String      m_reminder;
    String      m_altMessage;
    QuestReq    m_req;
    QuestResult m_results[6];
    Bytes       m_bytes;
};
static_assert(sizeof(QuestDetails) == 0x304, "QuestDetails size");

extern bool g_questCleanCR;
extern wchar_t g_questTypeBuf[32];
