#ifndef XML_PARSERS_H
#define XML_PARSERS_H
#include <tinyxml.h>
#include <qmatrix.h>
#include <vector>


#define CRAUSS_BEGIN_TAG "CRAUSSOPERATORS"
#define ID_VALUE_ATTR "ID_ATTR"
#define ID_ELEM_TAG "ID_ELEM"
#define MATRIX_ELEM_TAG "MATR_ELEM"
#define MATRIX_VALUE_RE_ATTR "RE_ATTR"
#define MATRIX_VALUE_IM_ATTR "IM_ATTR"
#define ID_BEGIN_TAG "ID_TAG"


/*------------ERRORS---------------*/
#define ERR_XML_LOAD_FAILED "Failed to load XML: "
#define ERR_QUERY_ELEM_FAILED "Failed to query element: "
#define ERR_INVALID_ELEMENTS_COUNT "Invalid elements count: "


class XmlParser
{
public:
    XmlParser(const char *path);
    ~XmlParser();
    std::vector<std::pair<QMatrix, std::vector<uint_type> > > GetOperators(uint8_t dim);
    std::vector<std::vector<int> > GetIds();
    void GetValue(const char *path, double val);
private:
    TiXmlDocument m_xmldoc;
};
#endif // XML_PARSERS_H
