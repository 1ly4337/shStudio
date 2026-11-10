#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Serializable.h"
#include "TableModel.h"

class iStream;
class oStream;

class DBTextTable : public Serializable, public TableModel
{
public:
    enum Split { kNone = 0, kByName = 1, kImplicitKey = 2, kLeadingKeys = 3 };

    DBTextTable() : m_codePage(1252), m_split(kNone), m_loaded(false) {}
    ~DBTextTable() override {}

    bool isEmpty() const override { return !m_loaded; }
    void clear() override;
    int getCodePage() const override { return (int)m_codePage; }
    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;

    bool parse(const BYTE* data, int size);
    std::string serialize() const;

    static UINT codePageForName(const wchar_t* fileName);
    HRESULT setCodePage(int codePage) override { m_codePage = codePage > 0 ? (UINT)codePage : 1252; return S_OK; }

    int rowCount() const override { return (int)m_rows.size(); }
    int fieldCount() const override { return (int)(m_keyCols.size() + m_strCols.size()); }
    int keyCount() const override { return (int)m_keyCols.size(); }
    bool isNumericField(int field) const override { return isKeyField(field); }
    std::wstring fieldName(int field) const override;
    std::wstring getText(int row, int field) const override;
    long long getKey(int row, int field) const;
    int setText(int row, int field, const wchar_t* text) override;
    int insertRow(int index) override;
    bool deleteRow(int index) override;
    Split split() const { return m_split; }
    const wchar_t* splitName() const;
    const std::wstring& error() const override { return m_error; }
    std::wstring statusText() const override;
    const wchar_t* typeName() const override { return L"a DB text table"; }
    std::wstring notEncodableText() const override;

private:
    struct Row
    {
        std::vector<long long> keys;
        std::vector<std::string> strs;
    };
    bool tryLayout(const BYTE* data, int size, int pos, int nKeys, int nStrs, std::vector<Row>& rows) const;
    std::wstring decode(const std::string& bytes) const;
    bool encode(const wchar_t* text, std::string& out) const;

    std::string               m_prefix;
    std::vector<std::wstring> m_columns;
    std::vector<int>          m_keyCols;
    std::vector<int>          m_strCols;
    std::vector<Row>          m_rows;
    UINT                      m_codePage;
    Split                     m_split;
    bool                      m_loaded;
    std::wstring              m_error;
};
