#pragma once
#include <string>

class TableModel
{
public:
    enum { kUnchanged = 0, kChanged = 1, kBadNumber = -1, kNotEncodable = -2, kBadCell = -3 };

    virtual ~TableModel() {}

    virtual int rowCount() const = 0;
    virtual int fieldCount() const = 0;
    virtual int keyCount() const = 0;
    bool isKeyField(int field) const { return field >= 0 && field < keyCount(); }
    virtual bool isNumericField(int field) const = 0;
    virtual std::wstring fieldName(int field) const = 0;
    virtual std::wstring getText(int row, int field) const = 0;
    virtual int setText(int row, int field, const wchar_t* text) = 0;
    virtual int insertRow(int index) = 0;
    virtual bool deleteRow(int index) = 0;

    virtual std::wstring statusText() const = 0;
    virtual const wchar_t* typeName() const = 0;
    virtual const std::wstring& error() const = 0;
    virtual std::wstring notEncodableText() const = 0;
    virtual bool prepareSave(std::wstring& error, bool* reordered) { *reordered = false; return true; }
};
