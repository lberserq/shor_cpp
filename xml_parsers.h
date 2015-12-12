#ifndef XML_PARSERS_H
#define XML_PARSERS_H
#include <tinyxml.h>
#include <qmatrix.h>
#include <vector>


#define CRAUSS_BEGIN_TAG "CRAUSSOPERATORS"
#define MATRIX_ELEM_TAG "MATR_ELEM"
#define MATRIX_VALUE_RE_ATTR "RE_ATTR"
#define MATRIX_VALUE_IM_ATTR "IM_ATTR"



/*------------ERRORS---------------*/
#define ERR_XML_LOAD_FAILED "Failed to load XML: "
#define ERR_QUERY_ELEM_FAILED "Failed to query element: "
#define ERR_INVALID_ELEMENTS_COUNT "Invalid elements count: "


class XmlParser
{
public:
    XmlParser(const char *path);
    ~XmlParser();
    std::vector<QMatrix> GetOperators(uint8_t dim);
    void GetValue(const char *path, double val);
private:
    TiXmlDocument m_xmldoc;
};
#endif // XML_PARSERS_H
