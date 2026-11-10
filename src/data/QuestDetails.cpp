#include "QuestDetails.h"
#include "SkillText.h"
#include "_w2_inl.h"
#include <cstring>
#include <cwchar>

bool    g_questCleanCR;
wchar_t g_questTypeBuf[32];

void QuestResult::clear()
{
    mu = 0; mc = 0;
    mk = 0;
    ic = 0; iu = 0;
    ik = 0;
    u2 = 0;
    u1 = 0;
    gold = 0;
    exp = 0;
    for (int k = 0; k < 3; ++k) {
        c[k] = 0;
        i[k] = 0;
        t[k] = 0;
    }
    unl = 0;
}

void QuestResult::read(iStream& s, int16_t version)
{
    mk = s.readByte();
    mu = s.readByte();
    mc = s.readByte();
    ik = s.readByte();
    ic = s.readByte();
    iu = s.readByte();
    u1   = s.readInt();
    u2   = s.readInt();
    exp  = s.readInt();
    gold = s.readInt();
    if ((mu != 0 || iu != 0) && mk == 0 && ik == 0)
        clear();
    t[0] = s.readByte();
    i[0] = s.readByte();
    if ((version & 0xFFF) >= 0x600) {
        c[0] = s.readByte();
        t[1] = s.readByte();
        i[1] = s.readByte();
        c[1] = s.readByte();
        t[2] = s.readByte();
        i[2] = s.readByte();
        c[2] = s.readByte();
    }
    unl = s.readShort();
}

oStream& QuestResult::write(oStream& s, int16_t version) const
{
    s << (char)mk << (char)mu << (char)mc << (char)ik << (char)ic << (char)iu;
    s << (unsigned long)u1 << (unsigned long)u2 << (unsigned long)exp << (unsigned long)gold;
    s << (char)t[0];
    if ((version & 0xFFF) >= 0x600) {
        s << (char)i[0] << (char)c[0] << (char)t[1] << (char)i[1] << (char)c[1] << (char)t[2] << (char)i[2];
        s << (char)c[2];
    } else {
        s << (char)i[0];
    }
    return s << (unsigned short)unl;
}

void QuestReq::read(iStream& s)
{
    minLevel = s.readShort();
    maxLevel = s.readShort();
    country = s.readByte();
    grow = s.readByte();
    na2 = s.readByte();
    na3 = s.readByte();
    classes[0] = s.readByte();
    classes[1] = s.readByte();
    classes[2] = s.readByte();
    classes[3] = s.readByte();
    classes[4] = s.readByte();
    classes[5] = s.readByte();
    prv2 = s.readShort();
    prv3 = s.readShort();
    na4 = s.readShort();
    na5 = s.readByte();
    prv1 = s.readShort();
    na6 = s.readInt();
    u1C = s.readInt();
    u20 = s.readShort();
    u22 = s.readByte();
    timer = s.readShort();
    u28 = s.readInt();
    u2C = s.readInt();
    u30 = s.readShort();
    rept = s.readByte();
    u33 = s.readByte();
    u34 = s.readShort();
    qmod = s.readByte();
    nty1 = s.readByte();
    nti1 = s.readShort();
    ity0 = s.readByte();
    iti0 = s.readByte();
    rty1 = s.readByte();
    rti1 = s.readByte();
    rcat1 = s.readByte();
    rty2 = s.readByte();
    rti2 = s.readByte();
    rcat2 = s.readByte();
    rty3 = s.readByte();
    rti3 = s.readByte();
    rcat3 = s.readByte();
    qtyp = s.readByte();
    nty2 = s.readByte();
    nti2 = s.readShort();
    fty1 = s.readByte();
    fti1 = s.readByte();
    fct1 = s.readByte();
    fty2 = s.readByte();
    fti2 = s.readByte();
    fct2 = s.readByte();
    fty3 = s.readByte();
    fti3 = s.readByte();
    fct3 = s.readByte();
    kpvp = s.readByte();
    mbi1 = s.readShort();
    mct1 = s.readByte();
    mbi2 = s.readShort();
    mct2 = s.readByte();
    drwd = s.readByte();
    clrw = s.readByte();
}

oStream& QuestReq::write(oStream& s) const
{
    s << (unsigned short)minLevel;
    s << (unsigned short)maxLevel;
    s << (char)country;
    s << (char)grow;
    s << (char)na2;
    s << (char)na3;
    s << (char)classes[0];
    s << (char)classes[1];
    s << (char)classes[2];
    s << (char)classes[3];
    s << (char)classes[4];
    s << (char)classes[5];
    s << (unsigned short)prv2;
    s << (unsigned short)prv3;
    s << (unsigned short)na4;
    s << (char)na5;
    s << (unsigned short)prv1;
    s << (unsigned long)na6;
    s << (unsigned long)u1C;
    s << (unsigned short)u20;
    s << (char)u22;
    s << (unsigned short)timer;
    s << (unsigned long)u28;
    s << (unsigned long)u2C;
    s << (unsigned short)u30;
    s << (char)rept;
    s << (char)u33;
    s << (unsigned short)u34;
    s << (char)qmod;
    s << (char)nty1;
    s << (unsigned short)nti1;
    s << (char)ity0;
    s << (char)iti0;
    s << (char)rty1;
    s << (char)rti1;
    s << (char)rcat1;
    s << (char)rty2;
    s << (char)rti2;
    s << (char)rcat2;
    s << (char)rty3;
    s << (char)rti3;
    s << (char)rcat3;
    s << (char)qtyp;
    s << (char)nty2;
    s << (unsigned short)nti2;
    s << (char)fty1;
    s << (char)fti1;
    s << (char)fct1;
    s << (char)fty2;
    s << (char)fti2;
    s << (char)fct2;
    s << (char)fty3;
    s << (char)fti3;
    s << (char)fct3;
    s << (char)kpvp;
    s << (unsigned short)mbi1;
    s << (char)mct1;
    s << (unsigned short)mbi2;
    s << (char)mct2;
    s << (char)drwd;
    s << (char)clrw;
    return s;
}

bool QuestReq::toText(String& str, int format) const
{
    if (format != 0x2D2D2D2D  && format != 0x74616273 )
        return false;
    char sepc = 9;
    if (format == 0x2D2D2D2D )
        sepc = 59;
    wchar_t sep = (wchar_t)(short)sepc;
    str << (wchar_t)sep;
    str << (short)minLevel;
    str << (wchar_t)sep;
    str << (short)maxLevel;
    str << (wchar_t)sep;
    str << (unsigned char)country;
    str << (wchar_t)sep;
    str << (unsigned char)grow;
    str << (wchar_t)sep;
    str << (unsigned char)na2;
    str << (wchar_t)sep;
    str << (unsigned char)na3;
    str << (wchar_t)sep;
    str << (unsigned char)classes[0];
    str << (wchar_t)sep;
    str << (unsigned char)classes[1];
    str << (wchar_t)sep;
    str << (unsigned char)classes[2];
    str << (wchar_t)sep;
    str << (unsigned char)classes[3];
    str << (wchar_t)sep;
    str << (unsigned char)classes[4];
    str << (wchar_t)sep;
    str << (unsigned char)classes[5];
    str << (wchar_t)sep;
    str << (short)prv2;
    str << (wchar_t)sep;
    str << (short)prv3;
    str << (wchar_t)sep;
    str << (short)na4;
    str << (wchar_t)sep;
    str << (unsigned char)na5;
    str << (wchar_t)sep;
    str << (short)prv1;
    str << (wchar_t)sep;
    str << (long)na6;
    str << (wchar_t)sep;
    str << (long)u1C;
    str << (wchar_t)sep;
    str << (short)u20;
    str << (wchar_t)sep;
    str << (unsigned char)u22;
    str << (wchar_t)sep;
    str << (short)timer;
    str << (wchar_t)sep;
    str << (long)u28;
    str << (wchar_t)sep;
    str << (long)u2C;
    str << (wchar_t)sep;
    str << (short)u30;
    str << (wchar_t)sep;
    str << (unsigned char)rept;
    str << (wchar_t)sep;
    str << (unsigned char)u33;
    str << (wchar_t)sep;
    str << (short)u34;
    str << (wchar_t)sep;
    str << (unsigned char)qmod;
    str << (wchar_t)sep;
    str << (unsigned char)nty1;
    str << (wchar_t)sep;
    str << (short)nti1;
    str << (wchar_t)sep;
    str << (unsigned char)ity0;
    str << (wchar_t)sep;
    str << (unsigned char)iti0;
    str << (wchar_t)sep;
    appendTabbedTriple(str, &rty1);
    str << (wchar_t)sep;
    appendTabbedTriple(str, &rty2);
    str << (wchar_t)sep;
    appendTabbedTriple(str, &rty3);
    str << (wchar_t)sep;
    str << (unsigned char)qtyp;
    str << (wchar_t)sep;
    str << (unsigned char)nty2;
    str << (wchar_t)sep;
    str << (short)nti2;
    str << (wchar_t)sep;
    appendTabbedTriple(str, &fty1);
    str << (wchar_t)sep;
    appendTabbedTriple(str, &fty2);
    str << (wchar_t)sep;
    appendTabbedTriple(str, &fty3);
    str << (wchar_t)sep;
    str << (unsigned char)kpvp;
    str << (wchar_t)sep;
    str << (short)mbi1;
    str << (wchar_t)sep;
    str << (unsigned char)mct1;
    str << (wchar_t)sep;
    str << (short)mbi2;
    str << (wchar_t)sep;
    str << (unsigned char)mct2;
    str << (wchar_t)sep;
    str << (unsigned char)drwd;
    str << (wchar_t)sep;
    str << (unsigned char)clrw;
    str << L"\r\n";
    return true;
}

QuestDetails::QuestDetails(int version)
    : m_bytes(0, (UINT)0)
{
    QuestDetails::reset();
    m_questId = 0;
    m_version = version;
}

QuestDetails::~QuestDetails()
{
}

void QuestDetails::reset()
{
    m_name.clear();
    m_summary.clear();
    for (int k = 0; k < 6; ++k)
        m_replies[k].clear();
    m_desc.clear();
    m_welcome.clear();
    m_reminder.clear();
    m_altMessage.clear();
    memset(&m_req, 0, 0x60);
    memset(m_results, 0, 0xD8);
}

void QuestDetails::cleanStrings()
{
    m_name.stripCrLf();
    m_name.stripRight(L' ');
    m_name.stripCrLf();
    m_summary.stripCrLf();
    m_summary.stripRight(L' ');
    m_summary.stripCrLf();
    for (int k = 0; k < 6; ++k) {
        m_replies[k].stripCrLf();
        m_replies[k].stripRight(L' ');
        m_replies[k].stripCrLf();
    }
    m_altMessage.stripCrLf();
    m_altMessage.stripRight(L' ');
    m_altMessage.stripCrLf();
    m_desc.stripCrLf();
    m_desc.stripRight(L' ');
    m_desc.stripCrLf();
    m_welcome.stripCrLf();
    m_welcome.stripRight(L' ');
    m_welcome.stripCrLf();
    m_reminder.stripCrLf();
    m_reminder.stripRight(L' ');
    m_reminder.stripCrLf();

    w2_strReplaceChar(m_name, 25, 39);
    w2_strReplaceChar(m_summary, 25, 39);
    for (int k = 0; k < 6; ++k)
        w2_strReplaceChar(m_replies[k], 25, 39);
    w2_strReplaceChar(m_altMessage, 25, 39);
    w2_strReplaceChar(m_desc, 25, 39);
    w2_strReplaceChar(m_welcome, 25, 39);
    w2_strReplaceChar(m_reminder, 25, 39);
}

void QuestDetails::readRawString(iStream& s, String& dst)
{
    int n = s.readDword();
    m_bytes.put(s, n);
    oStream& o = static_cast<oStream&>(m_bytes);
    o << (char)13 << (char)10;
    w2_rewind(s, n);
    dst.read(s, 4, static_cast<aStream&>(s).m_codePage);
}

static void w2_questReadRawInline(QuestDetails* q, iStream& s, String& dst)
{
    int n = s.readDword();
    q->m_bytes.put(s, n);
    oStream& o = static_cast<oStream&>(q->m_bytes);
    w2_putByte(o, 13);
    w2_putByte(o, 10);
    w2_rewind0(s, n);
    dst.read(s, 4, static_cast<aStream&>(s).m_codePage);
}

void QuestDetails::read(iStream& s, DWORD tag)
{
    if (tag == 0x66696C65 ) {
        aStream& a = static_cast<aStream&>(m_bytes);
        if (a.m_data && a.m_capacity)
            memset(a.m_data, 0, a.m_capacity);
        a.m_size = 0;
        a.m_pos = 0;
        m_questId = s.readShort();
        readRawString(s, m_name);
        readRawString(s, m_summary);
        m_req.read(s);
        if ((m_version & 0xFFF) < 0x600) {
            for (int k = 0; k < 3; ++k)
                m_results[k].read(s, (int16_t)m_version);
            for (int k = 0; k < 3; ++k)
                w2_questReadRawInline(this, s, m_replies[k]);
        } else {
            for (int k = 0; k < 6; ++k) {
                m_results[k].read(s, (int16_t)m_version);
                readRawString(s, m_replies[k]);
            }
        }
        w2_questReadRawInline(this, s, m_desc);
        w2_questReadRawInline(this, s, m_welcome);
        w2_questReadRawInline(this, s, m_reminder);
        w2_questReadRawInline(this, s, m_altMessage);
    } else if (tag == 0x666C6E74 ) {
        m_questId = s.readShort();
        m_req.read(s);
        for (int k = 0; k < 6; ++k)
            m_results[k].read(s, (int16_t)m_version);
        goto done;
    } else if (tag == 0x6C747874 ) {
        readRawString(s, m_name);
        readRawString(s, m_summary);
        readRawString(s, m_desc);
        readRawString(s, m_welcome);
        readRawString(s, m_reminder);
        readRawString(s, m_altMessage);
        for (int k = 0; k < 6; ++k)
            readRawString(s, m_replies[k]);
    }
    m_name = m_name.unescape();
    m_summary = m_summary.unescape();
    m_desc = m_desc.unescape();
    m_welcome = m_welcome.unescape();
    m_reminder = m_reminder.unescape();
    m_altMessage = m_altMessage.unescape();
    for (int k = 0; k < 6; ++k)
        m_replies[k] = m_replies[k].unescape();
done:
    if (g_questCleanCR)
        cleanStrings();
}

void QuestDetails::write(oStream& s, DWORD tag) const
{
    if (tag == 0x66696C65 ) {
        s << (unsigned short)m_questId;
        {
            String t = m_name.escape();
            t.write(s, 4, static_cast<aStream&>(s).m_codePage);
        }
        {
            String t = m_summary.escape();
            t.write(s, 4, static_cast<aStream&>(s).m_codePage);
        }
        m_req.write(s);
        if ((m_version & 0xFFF) < 0x600) {
            for (int k = 0; k < 3; ++k)
                m_results[k].write(s, (int16_t)m_version);
            for (int k = 0; k < 3; ++k) {
                String t = m_replies[k].escape();
                t.write(s, 4, 0xFFFFFFFF);
            }
        } else {
            for (int k = 0; k < 6; ++k) {
                m_results[k].write(s, (int16_t)m_version);
                String t = m_replies[k].escape();
                t.write(s, 4, 0xFFFFFFFF);
            }
        }
        String altm = m_altMessage.escape();
        String rmdr = m_reminder.escape();
        String welc = m_welcome.escape();
        String desc = m_desc.escape();
        desc.write(s, 4, static_cast<aStream&>(s).m_codePage);
        welc.write(s, 4, static_cast<aStream&>(s).m_codePage);
        rmdr.write(s, 4, static_cast<aStream&>(s).m_codePage);
        altm.write(s, 4, static_cast<aStream&>(s).m_codePage);
        return;
    }
    if (tag != 0x666C6E74 ) {
        if (tag != 0x6C747874 )
            return;
        {
            String summ = m_summary.escape();
            String name = m_name.escape();
            name.write(s, 4, static_cast<aStream&>(s).m_codePage);
            summ.write(s, 4, static_cast<aStream&>(s).m_codePage);
        }
        for (int k = 0; k < 6; ++k) {
            String t = m_replies[k].escape();
            t.write(s, 4, 0xFFFFFFFF);
        }
        String altm = m_altMessage.escape();
        String rmdr = m_reminder.escape();
        String welc = m_welcome.escape();
        String desc = m_desc.escape();
        desc.write(s, 4, static_cast<aStream&>(s).m_codePage);
        welc.write(s, 4, static_cast<aStream&>(s).m_codePage);
        rmdr.write(s, 4, static_cast<aStream&>(s).m_codePage);
        altm.write(s, 4, static_cast<aStream&>(s).m_codePage);
        return;
    }
    s << (unsigned short)m_questId;
    m_req.write(s);
    for (int k = 0; k < 6; ++k)
        m_results[k].write(s, (int16_t)m_version);
}

const wchar_t* QuestDetails::getString(DWORD tag) const
{
    switch (tag) {
    case 0x6E616D65 : return m_name.m_buffer;
    case 0x73756D6D : return m_summary.m_buffer;
    case 0x72657031 : return m_replies[0].m_buffer;
    case 0x72657032 : return m_replies[1].m_buffer;
    case 0x72657033 : return m_replies[2].m_buffer;
    case 0x72657034 : return m_replies[3].m_buffer;
    case 0x72657035 : return m_replies[4].m_buffer;
    case 0x72657036 : return m_replies[5].m_buffer;
    case 0x616C746D : return m_altMessage.m_buffer;
    case 0x64657363 : return m_desc.m_buffer;
    case 0x77656C63 : return m_welcome.m_buffer;
    case 0x726D6472 : return m_reminder.m_buffer;
    }
    if (tag != 0x71747970 )
        return 0;
    switch (m_req.qtyp) {
    case 0: return L"Final";
    case 1: return L"Farm";
    case 2: return L"PvE";
    case 3: return L"PvP";
    default:
        memset(g_questTypeBuf, 0, 32);
        swprintf_s(g_questTypeBuf, 0x20u, L"unk %i", m_req.qtyp);
        return g_questTypeBuf;
    }
}

int QuestDetails::getInt(DWORD tag) const
{
    switch (tag) {
    case 0x71756964 : return this->m_questId;
    case 0x6C6D696E : return this->m_req.minLevel;
    case 0x6C6D6178 : return this->m_req.maxLevel;
    case 0x636E7472 : return this->m_req.country;
    case 0x67726F77 : return this->m_req.grow;
    case 0x6E613220 : return this->m_req.na2;
    case 0x6E613320 : return this->m_req.na3;
    case 0x636C6131 : return this->m_req.classes[0];
    case 0x636C6132 : return this->m_req.classes[1];
    case 0x636C6133 : return this->m_req.classes[2];
    case 0x636C6134 : return this->m_req.classes[3];
    case 0x636C6135 : return this->m_req.classes[4];
    case 0x636C6136 : return this->m_req.classes[5];
    case 0x70727631 : return this->m_req.prv1;
    case 0x70727632 : return this->m_req.prv2;
    case 0x70727633 : return this->m_req.prv3;
    case 0x6E613420 : return this->m_req.na4;
    case 0x6E613520 : return this->m_req.na5;
    case 0x006E6136 : return this->m_req.na6;
    case 0x74696D72 : return this->m_req.timer;
    case 0x72657074 : return this->m_req.rept;
    case 0x716D6F64 : return this->m_req.qmod;
    case 0x6E747931 : return this->m_req.nty1;
    case 0x6E746931 : return this->m_req.nti1;
    case 0x69747930 : return this->m_req.ity0;
    case 0x69746930 : return this->m_req.iti0;
    case 0x71747970 : return this->m_req.qtyp;
    case 0x6E747932 : return this->m_req.nty2;
    case 0x6E746932 : return this->m_req.nti2;
    case 0x72747931 : return this->m_req.rty1;
    case 0x72746931 : return this->m_req.rti1;
    case 0x72637431 : return this->m_req.rcat1;
    case 0x72747932 : return this->m_req.rty2;
    case 0x72746932 : return this->m_req.rti2;
    case 0x72637432 : return this->m_req.rcat2;
    case 0x72747933 : return this->m_req.rty3;
    case 0x72746933 : return this->m_req.rti3;
    case 0x72637433 : return this->m_req.rcat3;
    case 0x66747931 : return this->m_req.fty1;
    case 0x66746931 : return this->m_req.fti1;
    case 0x66637431 : return this->m_req.fct1;
    case 0x66747932 : return this->m_req.fty2;
    case 0x66746932 : return this->m_req.fti2;
    case 0x66637432 : return this->m_req.fct2;
    case 0x66747933 : return this->m_req.fty3;
    case 0x66746933 : return this->m_req.fti3;
    case 0x66637433 : return this->m_req.fct3;
    case 0x6B707670 : return this->m_req.kpvp;
    case 0x6D626931 : return this->m_req.mbi1;
    case 0x6D626932 : return this->m_req.mbi2;
    case 0x6D637431 : return this->m_req.mct1;
    case 0x6D637432 : return this->m_req.mct2;
    case 0x64727764 : return this->m_req.drwd;
    case 0x636C7277 : return this->m_req.clrw;
    case 0x72316D6B : return this->m_results[0].mk;
    case 0x72326D6B : return this->m_results[1].mk;
    case 0x72336D6B : return this->m_results[2].mk;
    case 0x72346D6B : return this->m_results[3].mk;
    case 0x72356D6B : return this->m_results[4].mk;
    case 0x72366D6B : return this->m_results[5].mk;
    case 0x72316D75 : return this->m_results[0].mu;
    case 0x72326D75 : return this->m_results[1].mu;
    case 0x72336D75 : return this->m_results[2].mu;
    case 0x72346D75 : return this->m_results[3].mu;
    case 0x72356D75 : return this->m_results[4].mu;
    case 0x72366D75 : return this->m_results[5].mu;
    case 0x72316D63 : return this->m_results[0].mc;
    case 0x72326D63 : return this->m_results[1].mc;
    case 0x72336D63 : return this->m_results[2].mc;
    case 0x72346D63 : return this->m_results[3].mc;
    case 0x72356D63 : return this->m_results[4].mc;
    case 0x72366D63 : return this->m_results[5].mc;
    case 0x7231696B : return this->m_results[0].ik;
    case 0x7232696B : return this->m_results[1].ik;
    case 0x7233696B : return this->m_results[2].ik;
    case 0x7234696B : return this->m_results[3].ik;
    case 0x7235696B : return this->m_results[4].ik;
    case 0x7236696B : return this->m_results[5].ik;
    case 0x72316975 : return this->m_results[0].iu;
    case 0x72326975 : return this->m_results[1].iu;
    case 0x72336975 : return this->m_results[2].iu;
    case 0x72346975 : return this->m_results[3].iu;
    case 0x72356975 : return this->m_results[4].iu;
    case 0x72366975 : return this->m_results[5].iu;
    case 0x72316963 : return this->m_results[0].ic;
    case 0x72326963 : return this->m_results[1].ic;
    case 0x72336963 : return this->m_results[2].ic;
    case 0x72346963 : return this->m_results[3].ic;
    case 0x72356963 : return this->m_results[4].ic;
    case 0x72366963 : return this->m_results[5].ic;
    case 0x72317531 : return this->m_results[0].u1;
    case 0x72327531 : return this->m_results[1].u1;
    case 0x72337531 : return this->m_results[2].u1;
    case 0x72347531 : return this->m_results[3].u1;
    case 0x72357531 : return this->m_results[4].u1;
    case 0x72367531 : return this->m_results[5].u1;
    case 0x72317532 : return this->m_results[0].u2;
    case 0x72327532 : return this->m_results[1].u2;
    case 0x72337532 : return this->m_results[2].u2;
    case 0x72347532 : return this->m_results[3].u2;
    case 0x72357532 : return this->m_results[4].u2;
    case 0x72367532 : return this->m_results[5].u2;
    case 0x676C6431 : return this->m_results[0].gold;
    case 0x676C6432 : return this->m_results[1].gold;
    case 0x676C6433 : return this->m_results[2].gold;
    case 0x676C6434 : return this->m_results[3].gold;
    case 0x676C6435 : return this->m_results[4].gold;
    case 0x676C6436 : return this->m_results[5].gold;
    case 0x78703120 : return this->m_results[0].exp;
    case 0x78703220 : return this->m_results[1].exp;
    case 0x78703320 : return this->m_results[2].exp;
    case 0x78703420 : return this->m_results[3].exp;
    case 0x78703520 : return this->m_results[4].exp;
    case 0x78703620 : return this->m_results[5].exp;
    case 0x72317431 : return this->m_results[0].t[0];
    case 0x72317432 : return this->m_results[0].t[1];
    case 0x72317433 : return this->m_results[0].t[2];
    case 0x72327431 : return this->m_results[1].t[0];
    case 0x72327432 : return this->m_results[1].t[1];
    case 0x72327433 : return this->m_results[1].t[2];
    case 0x72337431 : return this->m_results[2].t[0];
    case 0x72337432 : return this->m_results[2].t[1];
    case 0x72337433 : return this->m_results[2].t[2];
    case 0x72347431 : return this->m_results[3].t[0];
    case 0x72347432 : return this->m_results[3].t[1];
    case 0x72347433 : return this->m_results[3].t[2];
    case 0x72357431 : return this->m_results[4].t[0];
    case 0x72357432 : return this->m_results[4].t[1];
    case 0x72357433 : return this->m_results[4].t[2];
    case 0x72367431 : return this->m_results[5].t[0];
    case 0x72367432 : return this->m_results[5].t[1];
    case 0x72367433 : return this->m_results[5].t[2];
    case 0x72316931 : return this->m_results[0].i[0];
    case 0x72316932 : return this->m_results[0].i[1];
    case 0x72316933 : return this->m_results[0].i[2];
    case 0x72326931 : return this->m_results[1].i[0];
    case 0x72326932 : return this->m_results[1].i[1];
    case 0x72326933 : return this->m_results[1].i[2];
    case 0x72336931 : return this->m_results[2].i[0];
    case 0x72336932 : return this->m_results[2].i[1];
    case 0x72336933 : return this->m_results[2].i[2];
    case 0x72346931 : return this->m_results[3].i[0];
    case 0x72346932 : return this->m_results[3].i[1];
    case 0x72346933 : return this->m_results[3].i[2];
    case 0x72356931 : return this->m_results[4].i[0];
    case 0x72356932 : return this->m_results[4].i[1];
    case 0x72356933 : return this->m_results[4].i[2];
    case 0x72366931 : return this->m_results[5].i[0];
    case 0x72366932 : return this->m_results[5].i[1];
    case 0x72366933 : return this->m_results[5].i[2];
    case 0x72316331 : return this->m_results[0].c[0];
    case 0x72316332 : return this->m_results[0].c[1];
    case 0x72316333 : return this->m_results[0].c[2];
    case 0x72326331 : return this->m_results[1].c[0];
    case 0x72326332 : return this->m_results[1].c[1];
    case 0x72326333 : return this->m_results[1].c[2];
    case 0x72336331 : return this->m_results[2].c[0];
    case 0x72336332 : return this->m_results[2].c[1];
    case 0x72336333 : return this->m_results[2].c[2];
    case 0x72346331 : return this->m_results[3].c[0];
    case 0x72346332 : return this->m_results[3].c[1];
    case 0x72346333 : return this->m_results[3].c[2];
    case 0x72356331 : return this->m_results[4].c[0];
    case 0x72356332 : return this->m_results[4].c[1];
    case 0x72356333 : return this->m_results[4].c[2];
    case 0x72366331 : return this->m_results[5].c[0];
    case 0x72366332 : return this->m_results[5].c[1];
    case 0x72366333 : return this->m_results[5].c[2];
    case 0x756E6C31 : return this->m_results[0].unl;
    case 0x756E6C32 : return this->m_results[1].unl;
    case 0x756E6C33 : return this->m_results[2].unl;
    case 0x756E6C34 : return this->m_results[3].unl;
    case 0x756E6C35 : return this->m_results[4].unl;
    case 0x756E6C36 : return this->m_results[5].unl;
    }
    return 0;
}

bool QuestDetails::setString(DWORD tag, const wchar_t* text)
{
    switch (tag) {
    case 0x6E616D65 : m_name = text; return true;
    case 0x73756D6D : m_summary = text; return true;
    case 0x72657031 : m_replies[0] = text; return true;
    case 0x72657032 : m_replies[1] = text; return true;
    case 0x72657033 : m_replies[2] = text; return true;
    case 0x72657034 : m_replies[3] = text; return true;
    case 0x72657035 : m_replies[4] = text; return true;
    case 0x72657036 : m_replies[5] = text; return true;
    case 0x616C746D : m_altMessage = text; return true;
    case 0x64657363 : m_desc = text; return true;
    case 0x77656C63 : m_welcome = text; return true;
    case 0x726D6472 : m_reminder = text; return true;
    }
    return false;
}

bool QuestDetails::setInt(DWORD tag, int a3)
{
    switch (tag) {
    case 0x71756964 : this->m_questId = (int16_t)a3; return true;
    case 0x6C6D696E : this->m_req.minLevel = (int16_t)a3; return true;
    case 0x6C6D6178 : this->m_req.maxLevel = (int16_t)a3; return true;
    case 0x636E7472 : this->m_req.country = (uint8_t)a3; return true;
    case 0x67726F77 : this->m_req.grow = (uint8_t)a3; return true;
    case 0x6E613220 : this->m_req.na2 = (uint8_t)a3; return true;
    case 0x6E613320 : this->m_req.na3 = (uint8_t)a3; return true;
    case 0x636C6131 : this->m_req.classes[0] = (uint8_t)a3; return true;
    case 0x636C6132 : this->m_req.classes[1] = (uint8_t)a3; return true;
    case 0x636C6133 : this->m_req.classes[2] = (uint8_t)a3; return true;
    case 0x636C6134 : this->m_req.classes[3] = (uint8_t)a3; return true;
    case 0x636C6135 : this->m_req.classes[4] = (uint8_t)a3; return true;
    case 0x636C6136 : this->m_req.classes[5] = (uint8_t)a3; return true;
    case 0x70727631 : this->m_req.prv1 = (int16_t)a3; return true;
    case 0x70727632 : this->m_req.prv2 = (int16_t)a3; return true;
    case 0x70727633 : this->m_req.prv3 = (int16_t)a3; return true;
    case 0x6E613420 : this->m_req.na4 = (int16_t)a3; return true;
    case 0x6E613520 : this->m_req.na5 = (uint8_t)a3; return true;
    case 0x006E6136 : this->m_req.na6 = a3; return true;
    case 0x74696D72 : this->m_req.timer = (int16_t)a3; return true;
    case 0x72657074 : this->m_req.rept = (uint8_t)a3; return true;
    case 0x716D6F64 : this->m_req.qmod = (uint8_t)a3; return true;
    case 0x6E747931 : this->m_req.nty1 = (uint8_t)a3; return true;
    case 0x6E746931 : this->m_req.nti1 = (int16_t)a3; return true;
    case 0x69747930 : this->m_req.ity0 = (uint8_t)a3; return true;
    case 0x69746930 : this->m_req.iti0 = (uint8_t)a3; return true;
    case 0x71747970 : this->m_req.qtyp = (uint8_t)a3; return true;
    case 0x6E747932 : this->m_req.nty2 = (uint8_t)a3; return true;
    case 0x6E746932 : this->m_req.nti2 = (int16_t)a3; return true;
    case 0x72747931 : this->m_req.rty1 = (uint8_t)a3; return true;
    case 0x72746931 : this->m_req.rti1 = (uint8_t)a3; return true;
    case 0x72637431 : this->m_req.rcat1 = (uint8_t)a3; return true;
    case 0x72747932 : this->m_req.rty2 = (uint8_t)a3; return true;
    case 0x72746932 : this->m_req.rti2 = (uint8_t)a3; return true;
    case 0x72637432 : this->m_req.rcat2 = (uint8_t)a3; return true;
    case 0x72747933 : this->m_req.rty3 = (uint8_t)a3; return true;
    case 0x72746933 : this->m_req.rti3 = (uint8_t)a3; return true;
    case 0x72637433 : this->m_req.rcat3 = (uint8_t)a3; return true;
    case 0x66747931 : this->m_req.fty1 = (uint8_t)a3; return true;
    case 0x66746931 : this->m_req.fti1 = (uint8_t)a3; return true;
    case 0x66637431 : this->m_req.fct1 = (uint8_t)a3; return true;
    case 0x66747932 : this->m_req.fty2 = (uint8_t)a3; return true;
    case 0x66746932 : this->m_req.fti2 = (uint8_t)a3; return true;
    case 0x66637432 : this->m_req.fct2 = (uint8_t)a3; return true;
    case 0x66747933 : this->m_req.fty3 = (uint8_t)a3; return true;
    case 0x66746933 : this->m_req.fti3 = (uint8_t)a3; return true;
    case 0x66637433 : this->m_req.fct3 = (uint8_t)a3; return true;
    case 0x6B707670 : this->m_req.kpvp = (uint8_t)a3; return true;
    case 0x6D626931 : this->m_req.mbi1 = (int16_t)a3; return true;
    case 0x6D626932 : this->m_req.mbi2 = (int16_t)a3; return true;
    case 0x6D637431 : this->m_req.mct1 = (uint8_t)a3; return true;
    case 0x6D637432 : this->m_req.mct2 = (uint8_t)a3; return true;
    case 0x64727764 : this->m_req.drwd = (uint8_t)a3; return true;
    case 0x636C7277 : this->m_req.clrw = (uint8_t)a3; return true;
    case 0x72316D6B : this->m_results[0].mk = (uint8_t)a3; return true;
    case 0x72326D6B : this->m_results[1].mk = (uint8_t)a3; return true;
    case 0x72336D6B : this->m_results[2].mk = (uint8_t)a3; return true;
    case 0x72346D6B : this->m_results[3].mk = (uint8_t)a3; return true;
    case 0x72356D6B : this->m_results[4].mk = (uint8_t)a3; return true;
    case 0x72366D6B : this->m_results[5].mk = (uint8_t)a3; return true;
    case 0x72316D75 : this->m_results[0].mu = (uint8_t)a3; return true;
    case 0x72326D75 : this->m_results[1].mu = (uint8_t)a3; return true;
    case 0x72336D75 : this->m_results[2].mu = (uint8_t)a3; return true;
    case 0x72346D75 : this->m_results[3].mu = (uint8_t)a3; return true;
    case 0x72356D75 : this->m_results[4].mu = (uint8_t)a3; return true;
    case 0x72366D75 : this->m_results[5].mu = (uint8_t)a3; return true;
    case 0x72316D63 : this->m_results[0].mc = (uint8_t)a3; return true;
    case 0x72326D63 : this->m_results[1].mc = (uint8_t)a3; return true;
    case 0x72336D63 : this->m_results[2].mc = (uint8_t)a3; return true;
    case 0x72346D63 : this->m_results[3].mc = (uint8_t)a3; return true;
    case 0x72356D63 : this->m_results[4].mc = (uint8_t)a3; return true;
    case 0x72366D63 : this->m_results[5].mc = (uint8_t)a3; return true;
    case 0x7231696B : this->m_results[0].ik = (uint8_t)a3; return true;
    case 0x7232696B : this->m_results[1].ik = (uint8_t)a3; return true;
    case 0x7233696B : this->m_results[2].ik = (uint8_t)a3; return true;
    case 0x7234696B : this->m_results[3].ik = (uint8_t)a3; return true;
    case 0x7235696B : this->m_results[4].ik = (uint8_t)a3; return true;
    case 0x7236696B : this->m_results[5].ik = (uint8_t)a3; return true;
    case 0x72316975 : this->m_results[0].iu = (uint8_t)a3; return true;
    case 0x72326975 : this->m_results[1].iu = (uint8_t)a3; return true;
    case 0x72336975 : this->m_results[2].iu = (uint8_t)a3; return true;
    case 0x72346975 : this->m_results[3].iu = (uint8_t)a3; return true;
    case 0x72356975 : this->m_results[4].iu = (uint8_t)a3; return true;
    case 0x72366975 : this->m_results[5].iu = (uint8_t)a3; return true;
    case 0x72316963 : this->m_results[0].ic = (uint8_t)a3; return true;
    case 0x72326963 : this->m_results[1].ic = (uint8_t)a3; return true;
    case 0x72336963 : this->m_results[2].ic = (uint8_t)a3; return true;
    case 0x72346963 : this->m_results[3].ic = (uint8_t)a3; return true;
    case 0x72356963 : this->m_results[4].ic = (uint8_t)a3; return true;
    case 0x72366963 : this->m_results[5].ic = (uint8_t)a3; return true;
    case 0x72317531 : this->m_results[0].u1 = a3; return true;
    case 0x72327531 : this->m_results[1].u1 = a3; return true;
    case 0x72337531 : this->m_results[2].u1 = a3; return true;
    case 0x72347531 : this->m_results[3].u1 = a3; return true;
    case 0x72357531 : this->m_results[4].u1 = a3; return true;
    case 0x72367531 : this->m_results[5].u1 = a3; return true;
    case 0x72317532 : this->m_results[0].u2 = a3; return true;
    case 0x72327532 : this->m_results[1].u2 = a3; return true;
    case 0x72337532 : this->m_results[2].u2 = a3; return true;
    case 0x72347532 : this->m_results[3].u2 = a3; return true;
    case 0x72357532 : this->m_results[4].u2 = a3; return true;
    case 0x72367532 : this->m_results[5].u2 = a3; return true;
    case 0x676C6431 : this->m_results[0].gold = a3; return true;
    case 0x676C6432 : this->m_results[1].gold = a3; return true;
    case 0x676C6433 : this->m_results[2].gold = a3; return true;
    case 0x676C6434 : this->m_results[3].gold = a3; return true;
    case 0x676C6435 : this->m_results[4].gold = a3; return true;
    case 0x676C6436 : this->m_results[5].gold = a3; return true;
    case 0x78703120 : this->m_results[0].exp = a3; return true;
    case 0x78703220 : this->m_results[1].exp = a3; return true;
    case 0x78703320 : this->m_results[2].exp = a3; return true;
    case 0x78703420 : this->m_results[3].exp = a3; return true;
    case 0x78703520 : this->m_results[4].exp = a3; return true;
    case 0x78703620 : this->m_results[5].exp = a3; return true;
    case 0x72317431 : this->m_results[0].t[0] = (uint8_t)a3; return true;
    case 0x72317432 : this->m_results[0].t[1] = (uint8_t)a3; return true;
    case 0x72317433 : this->m_results[0].t[2] = (uint8_t)a3; return true;
    case 0x72327431 : this->m_results[1].t[0] = (uint8_t)a3; return true;
    case 0x72327432 : this->m_results[1].t[1] = (uint8_t)a3; return true;
    case 0x72327433 : this->m_results[1].t[2] = (uint8_t)a3; return true;
    case 0x72337431 : this->m_results[2].t[0] = (uint8_t)a3; return true;
    case 0x72337432 : this->m_results[2].t[1] = (uint8_t)a3; return true;
    case 0x72337433 : this->m_results[2].t[2] = (uint8_t)a3; return true;
    case 0x72347431 : this->m_results[3].t[0] = (uint8_t)a3; return true;
    case 0x72347432 : this->m_results[3].t[1] = (uint8_t)a3; return true;
    case 0x72347433 : this->m_results[3].t[2] = (uint8_t)a3; return true;
    case 0x72357431 : this->m_results[4].t[0] = (uint8_t)a3; return true;
    case 0x72357432 : this->m_results[4].t[1] = (uint8_t)a3; return true;
    case 0x72357433 : this->m_results[4].t[2] = (uint8_t)a3; return true;
    case 0x72367431 : this->m_results[5].t[0] = (uint8_t)a3; return true;
    case 0x72367432 : this->m_results[5].t[1] = (uint8_t)a3; return true;
    case 0x72367433 : this->m_results[5].t[2] = (uint8_t)a3; return true;
    case 0x72316931 : this->m_results[0].i[0] = (uint8_t)a3; return true;
    case 0x72316932 : this->m_results[0].i[1] = (uint8_t)a3; return true;
    case 0x72316933 : this->m_results[0].i[2] = (uint8_t)a3; return true;
    case 0x72326931 : this->m_results[1].i[0] = (uint8_t)a3; return true;
    case 0x72326932 : this->m_results[1].i[1] = (uint8_t)a3; return true;
    case 0x72326933 : this->m_results[1].i[2] = (uint8_t)a3; return true;
    case 0x72336931 : this->m_results[2].i[0] = (uint8_t)a3; return true;
    case 0x72336932 : this->m_results[2].i[1] = (uint8_t)a3; return true;
    case 0x72336933 : this->m_results[2].i[2] = (uint8_t)a3; return true;
    case 0x72346931 : this->m_results[3].i[0] = (uint8_t)a3; return true;
    case 0x72346932 : this->m_results[3].i[1] = (uint8_t)a3; return true;
    case 0x72346933 : this->m_results[3].i[2] = (uint8_t)a3; return true;
    case 0x72356931 : this->m_results[4].i[0] = (uint8_t)a3; return true;
    case 0x72356932 : this->m_results[4].i[1] = (uint8_t)a3; return true;
    case 0x72356933 : this->m_results[4].i[2] = (uint8_t)a3; return true;
    case 0x72366931 : this->m_results[5].i[0] = (uint8_t)a3; return true;
    case 0x72366932 : this->m_results[5].i[1] = (uint8_t)a3; return true;
    case 0x72366933 : this->m_results[5].i[2] = (uint8_t)a3; return true;
    case 0x72316331 : this->m_results[0].c[0] = (uint8_t)a3; return true;
    case 0x72316332 : this->m_results[0].c[1] = (uint8_t)a3; return true;
    case 0x72316333 : this->m_results[0].c[2] = (uint8_t)a3; return true;
    case 0x72326331 : this->m_results[1].c[0] = (uint8_t)a3; return true;
    case 0x72326332 : this->m_results[1].c[1] = (uint8_t)a3; return true;
    case 0x72326333 : this->m_results[1].c[2] = (uint8_t)a3; return true;
    case 0x72336331 : this->m_results[2].c[0] = (uint8_t)a3; return true;
    case 0x72336332 : this->m_results[2].c[1] = (uint8_t)a3; return true;
    case 0x72336333 : this->m_results[2].c[2] = (uint8_t)a3; return true;
    case 0x72346331 : this->m_results[3].c[0] = (uint8_t)a3; return true;
    case 0x72346332 : this->m_results[3].c[1] = (uint8_t)a3; return true;
    case 0x72346333 : this->m_results[3].c[2] = (uint8_t)a3; return true;
    case 0x72356331 : this->m_results[4].c[0] = (uint8_t)a3; return true;
    case 0x72356332 : this->m_results[4].c[1] = (uint8_t)a3; return true;
    case 0x72356333 : this->m_results[4].c[2] = (uint8_t)a3; return true;
    case 0x72366331 : this->m_results[5].c[0] = (uint8_t)a3; return true;
    case 0x72366332 : this->m_results[5].c[1] = (uint8_t)a3; return true;
    case 0x72366333 : this->m_results[5].c[2] = (uint8_t)a3; return true;
    case 0x756E6C31 : this->m_results[0].unl = (int16_t)a3; return true;
    case 0x756E6C32 : this->m_results[1].unl = (int16_t)a3; return true;
    case 0x756E6C33 : this->m_results[2].unl = (int16_t)a3; return true;
    case 0x756E6C34 : this->m_results[3].unl = (int16_t)a3; return true;
    case 0x756E6C35 : this->m_results[4].unl = (int16_t)a3; return true;
    case 0x756E6C36 : this->m_results[5].unl = (int16_t)a3; return true;
    }
    return true;
}

int QuestDetails::compare(const Node* otherNode, DWORD tag) const
{
    const QuestDetails* other = &dynamic_cast<const QuestDetails&>(*otherNode);
    switch (tag) {
    case 0x71756964 : return w2_cmpS(this->m_questId, other->m_questId);
    case 0x6C6D696E : return w2_cmpS(this->m_req.minLevel, other->m_req.minLevel);
    case 0x6C6D6178 : return w2_cmpS(this->m_req.maxLevel, other->m_req.maxLevel);
    case 0x636E7472 : return w2_cmpU(this->m_req.country, other->m_req.country);
    case 0x67726F77 : return w2_cmpU(this->m_req.grow, other->m_req.grow);
    case 0x6E613220 : return w2_cmpU(this->m_req.na2, other->m_req.na2);
    case 0x6E613320 : return w2_cmpU(this->m_req.na3, other->m_req.na3);
    case 0x636C6131 : return w2_cmpU(this->m_req.classes[0], other->m_req.classes[0]);
    case 0x636C6132 : return w2_cmpU(this->m_req.classes[1], other->m_req.classes[1]);
    case 0x636C6133 : return w2_cmpU(this->m_req.classes[2], other->m_req.classes[2]);
    case 0x636C6134 : return w2_cmpU(this->m_req.classes[3], other->m_req.classes[3]);
    case 0x636C6135 : return w2_cmpU(this->m_req.classes[4], other->m_req.classes[4]);
    case 0x636C6136 : return w2_cmpU(this->m_req.classes[5], other->m_req.classes[5]);
    case 0x70727631 : return w2_cmpS(this->m_req.prv1, other->m_req.prv1);
    case 0x70727632 : return w2_cmpS(this->m_req.prv2, other->m_req.prv2);
    case 0x70727633 : return w2_cmpS(this->m_req.prv3, other->m_req.prv3);
    case 0x6E613420 : return w2_cmpS(this->m_req.na4, other->m_req.na4);
    case 0x6E613520 : return w2_cmpU(this->m_req.na5, other->m_req.na5);
    case 0x006E6136 : return w2_cmpS(this->m_req.na6, other->m_req.na6);
    case 0x74696D72 : return w2_cmpS(this->m_req.timer, other->m_req.timer);
    case 0x72657074 : return w2_cmpU(this->m_req.rept, other->m_req.rept);
    case 0x716D6F64 : return w2_cmpU(this->m_req.qmod, other->m_req.qmod);
    case 0x6E747931 : return w2_cmpU(this->m_req.nty1, other->m_req.nty1);
    case 0x6E746931 : return w2_cmpS(this->m_req.nti1, other->m_req.nti1);
    case 0x69747930 : return w2_cmpU(this->m_req.ity0, other->m_req.ity0);
    case 0x69746930 : return w2_cmpU(this->m_req.iti0, other->m_req.iti0);
    case 0x71747970 : return w2_cmpU(this->m_req.qtyp, other->m_req.qtyp);
    case 0x6E747932 : return w2_cmpU(this->m_req.nty2, other->m_req.nty2);
    case 0x6E746932 : return w2_cmpS(this->m_req.nti2, other->m_req.nti2);
    case 0x72747931 : return w2_cmpU(this->m_req.rty1, other->m_req.rty1);
    case 0x72747932 : return w2_cmpU(this->m_req.rty2, other->m_req.rty2);
    case 0x72747933 : return w2_cmpU(this->m_req.rty3, other->m_req.rty3);
    case 0x72746931 : return w2_cmpU(this->m_req.rti1, other->m_req.rti1);
    case 0x72746932 : return w2_cmpU(this->m_req.rti2, other->m_req.rti2);
    case 0x72746933 : return w2_cmpU(this->m_req.rti3, other->m_req.rti3);
    case 0x72637431 : return w2_cmpU(this->m_req.rcat1, other->m_req.rcat1);
    case 0x72637432 : return w2_cmpU(this->m_req.rcat2, other->m_req.rcat2);
    case 0x72637433 : return w2_cmpU(this->m_req.rcat3, other->m_req.rcat3);
    case 0x66747931 : return w2_cmpU(this->m_req.fty1, other->m_req.fty1);
    case 0x66747932 : return w2_cmpU(this->m_req.fty2, other->m_req.fty2);
    case 0x66747933 : return w2_cmpU(this->m_req.fty3, other->m_req.fty3);
    case 0x66746931 : return w2_cmpU(this->m_req.fti1, other->m_req.fti1);
    case 0x66746932 : return w2_cmpU(this->m_req.fti2, other->m_req.fti2);
    case 0x66746933 : return w2_cmpU(this->m_req.fti3, other->m_req.fti3);
    case 0x66637431 : return w2_cmpU(this->m_req.fct1, other->m_req.fct1);
    case 0x66637432 : return w2_cmpU(this->m_req.fct2, other->m_req.fct2);
    case 0x66637433 : return w2_cmpU(this->m_req.fct3, other->m_req.fct3);
    case 0x6B707670 : return w2_cmpU(this->m_req.kpvp, other->m_req.kpvp);
    case 0x6D626931 : return w2_cmpS(this->m_req.mbi1, other->m_req.mbi1);
    case 0x6D626932 : return w2_cmpS(this->m_req.mbi2, other->m_req.mbi2);
    case 0x6D637431 : return w2_cmpU(this->m_req.mct1, other->m_req.mct1);
    case 0x6D637432 : return w2_cmpU(this->m_req.mct2, other->m_req.mct2);
    case 0x64727764 : return w2_cmpU(this->m_req.drwd, other->m_req.drwd);
    case 0x636C7277 : return w2_cmpU(this->m_req.clrw, other->m_req.clrw);
    case 0x6E616D65 : return String::compareLogical(this->m_name, other->m_name);
    case 0x72316D6B : return w2_cmpU(this->m_results[0].mk, other->m_results[0].mk);
    case 0x72326D6B : return w2_cmpU(this->m_results[1].mk, other->m_results[1].mk);
    case 0x72336D6B : return w2_cmpU(this->m_results[2].mk, other->m_results[2].mk);
    case 0x72346D6B : return w2_cmpU(this->m_results[3].mk, other->m_results[3].mk);
    case 0x72356D6B : return w2_cmpU(this->m_results[4].mk, other->m_results[4].mk);
    case 0x72366D6B : return w2_cmpU(this->m_results[5].mk, other->m_results[5].mk);
    case 0x72316D75 : return w2_cmpU(this->m_results[0].mu, other->m_results[0].mu);
    case 0x72326D75 : return w2_cmpU(this->m_results[1].mu, other->m_results[1].mu);
    case 0x72336D75 : return w2_cmpU(this->m_results[2].mu, other->m_results[2].mu);
    case 0x72346D75 : return w2_cmpU(this->m_results[3].mu, other->m_results[3].mu);
    case 0x72356D75 : return w2_cmpU(this->m_results[4].mu, other->m_results[4].mu);
    case 0x72366D75 : return w2_cmpU(this->m_results[5].mu, other->m_results[5].mu);
    case 0x72316D63 : return w2_cmpU(this->m_results[0].mc, other->m_results[0].mc);
    case 0x72326D63 : return w2_cmpU(this->m_results[1].mc, other->m_results[1].mc);
    case 0x72336D63 : return w2_cmpU(this->m_results[2].mc, other->m_results[2].mc);
    case 0x72346D63 : return w2_cmpU(this->m_results[3].mc, other->m_results[3].mc);
    case 0x72356D63 : return w2_cmpU(this->m_results[4].mc, other->m_results[4].mc);
    case 0x72366D63 : return w2_cmpU(this->m_results[5].mc, other->m_results[5].mc);
    case 0x7231696B : return w2_cmpU(this->m_results[0].ik, other->m_results[0].ik);
    case 0x7232696B : return w2_cmpU(this->m_results[1].ik, other->m_results[1].ik);
    case 0x7233696B : return w2_cmpU(this->m_results[2].ik, other->m_results[2].ik);
    case 0x7234696B : return w2_cmpU(this->m_results[3].ik, other->m_results[3].ik);
    case 0x7235696B : return w2_cmpU(this->m_results[4].ik, other->m_results[4].ik);
    case 0x7236696B : return w2_cmpU(this->m_results[5].ik, other->m_results[5].ik);
    case 0x72316975 : return w2_cmpU(this->m_results[0].iu, other->m_results[0].iu);
    case 0x72326975 : return w2_cmpU(this->m_results[1].iu, other->m_results[1].iu);
    case 0x72336975 : return w2_cmpU(this->m_results[2].iu, other->m_results[2].iu);
    case 0x72346975 : return w2_cmpU(this->m_results[3].iu, other->m_results[3].iu);
    case 0x72356975 : return w2_cmpU(this->m_results[4].iu, other->m_results[4].iu);
    case 0x72366975 : return w2_cmpU(this->m_results[5].iu, other->m_results[5].iu);
    case 0x72316963 : return w2_cmpU(this->m_results[0].ic, other->m_results[0].ic);
    case 0x72326963 : return w2_cmpU(this->m_results[1].ic, other->m_results[1].ic);
    case 0x72336963 : return w2_cmpU(this->m_results[2].ic, other->m_results[2].ic);
    case 0x72346963 : return w2_cmpU(this->m_results[3].ic, other->m_results[3].ic);
    case 0x72356963 : return w2_cmpU(this->m_results[4].ic, other->m_results[4].ic);
    case 0x72366963 : return w2_cmpU(this->m_results[5].ic, other->m_results[5].ic);
    case 0x72317531 : return w2_cmpS(this->m_results[0].u1, other->m_results[0].u1);
    case 0x72327531 : return w2_cmpS(this->m_results[1].u1, other->m_results[1].u1);
    case 0x72337531 : return w2_cmpS(this->m_results[2].u1, other->m_results[2].u1);
    case 0x72347531 : return w2_cmpS(this->m_results[3].u1, other->m_results[3].u1);
    case 0x72357531 : return w2_cmpS(this->m_results[4].u1, other->m_results[4].u1);
    case 0x72367531 : return w2_cmpS(this->m_results[5].u1, other->m_results[5].u1);
    case 0x72317532 : return w2_cmpS(this->m_results[0].u2, other->m_results[0].u2);
    case 0x72327532 : return w2_cmpS(this->m_results[1].u2, other->m_results[1].u2);
    case 0x72337532 : return w2_cmpS(this->m_results[2].u2, other->m_results[2].u2);
    case 0x72347532 : return w2_cmpS(this->m_results[3].u2, other->m_results[3].u2);
    case 0x72357532 : return w2_cmpS(this->m_results[4].u2, other->m_results[4].u2);
    case 0x72367532 : return w2_cmpS(this->m_results[5].u2, other->m_results[5].u2);
    case 0x676C6431 : return w2_cmpS(this->m_results[0].gold, other->m_results[0].gold);
    case 0x676C6432 : return w2_cmpS(this->m_results[1].gold, other->m_results[1].gold);
    case 0x676C6433 : return w2_cmpS(this->m_results[2].gold, other->m_results[2].gold);
    case 0x676C6434 : return w2_cmpS(this->m_results[3].gold, other->m_results[3].gold);
    case 0x676C6435 : return w2_cmpS(this->m_results[4].gold, other->m_results[4].gold);
    case 0x676C6436 : return w2_cmpS(this->m_results[5].gold, other->m_results[5].gold);
    case 0x78703120 : return w2_cmpS(this->m_results[0].exp, other->m_results[0].exp);
    case 0x78703220 : return w2_cmpS(this->m_results[1].exp, other->m_results[1].exp);
    case 0x78703320 : return w2_cmpS(this->m_results[2].exp, other->m_results[2].exp);
    case 0x78703420 : return w2_cmpS(this->m_results[3].exp, other->m_results[3].exp);
    case 0x78703520 : return w2_cmpS(this->m_results[4].exp, other->m_results[4].exp);
    case 0x78703620 : return w2_cmpS(this->m_results[5].exp, other->m_results[5].exp);
    case 0x72317431 : return w2_cmpU(this->m_results[0].t[0], other->m_results[0].t[0]);
    case 0x72317432 : return w2_cmpU(this->m_results[0].t[1], other->m_results[0].t[1]);
    case 0x72317433 : return w2_cmpU(this->m_results[0].t[2], other->m_results[0].t[2]);
    case 0x72327431 : return w2_cmpU(this->m_results[1].t[0], other->m_results[1].t[0]);
    case 0x72327432 : return w2_cmpU(this->m_results[1].t[1], other->m_results[1].t[1]);
    case 0x72327433 : return w2_cmpU(this->m_results[1].t[2], other->m_results[1].t[2]);
    case 0x72337431 : return w2_cmpU(this->m_results[2].t[0], other->m_results[2].t[0]);
    case 0x72337432 : return w2_cmpU(this->m_results[2].t[1], other->m_results[2].t[1]);
    case 0x72337433 : return w2_cmpU(this->m_results[2].t[2], other->m_results[2].t[2]);
    case 0x72347431 : return w2_cmpU(this->m_results[3].t[0], other->m_results[3].t[0]);
    case 0x72347432 : return w2_cmpU(this->m_results[3].t[1], other->m_results[3].t[1]);
    case 0x72347433 : return w2_cmpU(this->m_results[3].t[2], other->m_results[3].t[2]);
    case 0x72357431 : return w2_cmpU(this->m_results[4].t[0], other->m_results[4].t[0]);
    case 0x72357432 : return w2_cmpU(this->m_results[4].t[1], other->m_results[4].t[1]);
    case 0x72357433 : return w2_cmpU(this->m_results[4].t[2], other->m_results[4].t[2]);
    case 0x72367431 : return w2_cmpU(this->m_results[5].t[0], other->m_results[5].t[0]);
    case 0x72367432 : return w2_cmpU(this->m_results[5].t[1], other->m_results[5].t[1]);
    case 0x72367433 : return w2_cmpU(this->m_results[5].t[2], other->m_results[5].t[2]);
    case 0x72316931 : return w2_cmpU(this->m_results[0].i[0], other->m_results[0].i[0]);
    case 0x72316932 : return w2_cmpU(this->m_results[0].i[1], other->m_results[0].i[1]);
    case 0x72316933 : return w2_cmpU(this->m_results[0].i[2], other->m_results[0].i[2]);
    case 0x72326931 : return w2_cmpU(this->m_results[1].i[0], other->m_results[1].i[0]);
    case 0x72326932 : return w2_cmpU(this->m_results[1].i[1], other->m_results[1].i[1]);
    case 0x72326933 : return w2_cmpU(this->m_results[1].i[2], other->m_results[1].i[2]);
    case 0x72336931 : return w2_cmpU(this->m_results[2].i[0], other->m_results[2].i[0]);
    case 0x72336932 : return w2_cmpU(this->m_results[2].i[1], other->m_results[2].i[1]);
    case 0x72336933 : return w2_cmpU(this->m_results[2].i[2], other->m_results[2].i[2]);
    case 0x72346931 : return w2_cmpU(this->m_results[3].i[0], other->m_results[3].i[0]);
    case 0x72346932 : return w2_cmpU(this->m_results[3].i[1], other->m_results[3].i[1]);
    case 0x72346933 : return w2_cmpU(this->m_results[3].i[2], other->m_results[3].i[2]);
    case 0x72356931 : return w2_cmpU(this->m_results[4].i[0], other->m_results[4].i[0]);
    case 0x72356932 : return w2_cmpU(this->m_results[4].i[1], other->m_results[4].i[1]);
    case 0x72356933 : return w2_cmpU(this->m_results[4].i[2], other->m_results[4].i[2]);
    case 0x72366931 : return w2_cmpU(this->m_results[5].i[0], other->m_results[5].i[0]);
    case 0x72366932 : return w2_cmpU(this->m_results[5].i[1], other->m_results[5].i[1]);
    case 0x72366933 : return w2_cmpU(this->m_results[5].i[2], other->m_results[5].i[2]);
    case 0x72316331 : return w2_cmpU(this->m_results[0].c[0], other->m_results[0].c[0]);
    case 0x72316332 : return w2_cmpU(this->m_results[0].c[1], other->m_results[0].c[1]);
    case 0x72316333 : return w2_cmpU(this->m_results[0].c[2], other->m_results[0].c[2]);
    case 0x72326331 : return w2_cmpU(this->m_results[1].c[0], other->m_results[1].c[0]);
    case 0x72326332 : return w2_cmpU(this->m_results[1].c[1], other->m_results[1].c[1]);
    case 0x72326333 : return w2_cmpU(this->m_results[1].c[2], other->m_results[1].c[2]);
    case 0x72336331 : return w2_cmpU(this->m_results[2].c[0], other->m_results[2].c[0]);
    case 0x72336332 : return w2_cmpU(this->m_results[2].c[1], other->m_results[2].c[1]);
    case 0x72336333 : return w2_cmpU(this->m_results[2].c[2], other->m_results[2].c[2]);
    case 0x72346331 : return w2_cmpU(this->m_results[3].c[0], other->m_results[3].c[0]);
    case 0x72346332 : return w2_cmpU(this->m_results[3].c[1], other->m_results[3].c[1]);
    case 0x72346333 : return w2_cmpU(this->m_results[3].c[2], other->m_results[3].c[2]);
    case 0x72356331 : return w2_cmpU(this->m_results[4].c[0], other->m_results[4].c[0]);
    case 0x72356332 : return w2_cmpU(this->m_results[4].c[1], other->m_results[4].c[1]);
    case 0x72356333 : return w2_cmpU(this->m_results[4].c[2], other->m_results[4].c[2]);
    case 0x72366331 : return w2_cmpU(this->m_results[5].c[0], other->m_results[5].c[0]);
    case 0x72366332 : return w2_cmpU(this->m_results[5].c[1], other->m_results[5].c[1]);
    case 0x72366333 : return w2_cmpU(this->m_results[5].c[2], other->m_results[5].c[2]);
    case 0x756E6C31 : return w2_cmpS(this->m_results[0].unl, other->m_results[0].unl);
    case 0x756E6C32 : return w2_cmpS(this->m_results[1].unl, other->m_results[1].unl);
    case 0x756E6C33 : return w2_cmpS(this->m_results[2].unl, other->m_results[2].unl);
    case 0x756E6C34 : return w2_cmpS(this->m_results[3].unl, other->m_results[3].unl);
    case 0x756E6C35 : return w2_cmpS(this->m_results[4].unl, other->m_results[4].unl);
    case 0x756E6C36 : return w2_cmpS(this->m_results[5].unl, other->m_results[5].unl);
    }
    return 0;
}

bool QuestDetails::toText(String& out, DWORD format) const
{
    if (format != 0x2D2D2D2D  && format != 0x74616273 )
        return false;
    char sepc = 9;
    if (format == 0x2D2D2D2D )
        sepc = 59;
    wchar_t sep = (wchar_t)(short)sepc;
    out << (short)m_questId;
    out << (wchar_t)sep;
    return m_req.toText(out, format);
}
