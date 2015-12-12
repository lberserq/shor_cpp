#include <config.h>
#include <cstdlib>
#include "xml_parsers.h"

XmlParser::XmlParser(const char *path)
{
    if (!m_xmldoc.LoadFile(path)) {
        throw std::logic_error(std::string(std::string(ERR_XML_LOAD_FAILED) + path
                                           + ": "
                                           + m_xmldoc.ErrorDesc()
                                           ).c_str());
    }
}

std::vector<QMatrix> XmlParser::GetOperators(uint8_t dim)
{
    TiXmlElement *pRoot, *pParam;
    std::vector<QMatrix> resV;
    pRoot = m_xmldoc.FirstChildElement(CRAUSS_BEGIN_TAG);
    if (pRoot)
    {
        std::string resTag;
        resTag = static_cast<char>('A' + dim);
        pParam = pRoot->FirstChildElement(resTag);
        while (pParam)
        {

            TiXmlElement *pVal = pParam->FirstChildElement(MATRIX_ELEM_TAG);
            int len = (1 << dim);
            QMatrix matrix(len, len);
            for (int row = 0; row < len; row++) {
                for (int column = 0; column < len; column++) {
                    mcomplex tmp;
                    double re = 0, im = 0;
                    if (pVal->QueryDoubleAttribute(MATRIX_VALUE_RE_ATTR, &re)
                            != TIXML_SUCCESS) {
                        throw std::logic_error(ERR_QUERY_ELEM_FAILED);
                    }

                    if (pVal->QueryDoubleAttribute(MATRIX_VALUE_IM_ATTR, &im)
                            != TIXML_SUCCESS) {
                        throw std::logic_error(ERR_QUERY_ELEM_FAILED);
                    }
                    tmp = mcomplex(re, im);
                    matrix(row, column) = tmp;
                    pVal = pVal->NextSiblingElement(MATRIX_ELEM_TAG);
                    if (!pVal && row != column && row != len - 1) {
                        char rowC = static_cast<char>('0' + row);
                        char columnC = static_cast<char>('0' + column);
                        throw std::logic_error(std::string(
                                                   std::string(ERR_INVALID_ELEMENTS_COUNT)
                                                   + rowC + "x" + columnC).c_str()
                                               );
                    }
                }
            }
            resV.push_back(matrix);
            pParam = pParam->NextSiblingElement(resTag);
        }
    }
    return resV;

}



XmlParser::~XmlParser()
{
    m_xmldoc.Clear();
}
