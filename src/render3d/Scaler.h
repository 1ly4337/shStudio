#pragma once
#include <windows.h>

class Document;
class SpotDetails;
template <class T> class TabCtrl;

class Scaler
{
public:
    Scaler(Document* document, TabCtrl<SpotDetails*>* tabs) : m_document(document), m_tabs(tabs) {}

    virtual POINT* toPoint(POINT* out, float x, float z) = 0;

public:
    Document*              m_document;
    TabCtrl<SpotDetails*>* m_tabs;
};
static_assert(sizeof(Scaler) == 0x0C, "Scaler size");
