#include <common/config.h>
#include <cstdlib>
#include <common/infra/qscript_stubs.h>
#include <common/config.h>
#include <common/infra/xmlparsers.h>

XmlParser::XmlParser(const char *path)
{
    if (!m_xmldoc.LoadFile(path)) {
        throw std::logic_error(std::string(std::string(ERR_XML_LOAD_FAILED) + path
                                           + ": "
                                           + m_xmldoc.ErrorDesc()
                                           ).c_str());
    }
}

std::vector<std::pair<QMatrix, std::vector<uint_type> > > XmlParser::GetOperators(uint8_t dim)
{
    TiXmlElement *pRoot, *pParam;
    typedef std::vector<std::pair<QMatrix, std::vector<uint_type> > > resType;
    resType resV;

    if (dim == 0) {
        return resV;
    }
    pRoot = m_xmldoc.FirstChildElement(CRAUSS_BEGIN_TAG);
    if (pRoot)
    {
        std::string resTag;

        resTag = std::string("MAT") + (static_cast<const char>('0' + dim));
        pParam = pRoot->FirstChildElement(resTag.c_str());
        while (pParam)
        {

            TiXmlElement *pVal = pParam->FirstChildElement(MATRIX_ELEM_TAG);
            int len = (1 << dim);
            QMatrix matrix(len, len);
            if (pVal) {
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
                                                       + rowC + "x" + columnC)
                                                       .c_str());
                        }
                    }
                }
            }

            //load ids
            std::vector<uint_type> ids;
            pVal = pParam->FirstChildElement(ID_ELEM_TAG);
            if (pVal) {
                for (uint8_t i = 0; i < dim; ++i) {
                    unsigned val;
                    if (pVal->QueryUnsignedAttribute(ID_VALUE_ATTR, &val)
                            != TIXML_SUCCESS) {
                        throw std::logic_error(ERR_QUERY_ELEM_FAILED);
                    }
                    ids.push_back(val);
                    pVal = pVal->NextSiblingElement(ID_ELEM_TAG);
                    if (!pVal && i != dim - 1) {
                        throw std::logic_error("Not enough data");
                    }
                }
            }
            std::pair<QMatrix, std::vector<uint_type> > elem = std::make_pair(matrix, ids);
            resV.push_back(elem);

            pParam = pParam->NextSiblingElement(resTag.c_str());
        }
    }

    ParallelSubSystemHelper::barrier();
    return resV;
}




XmlParser::~XmlParser()
{
    m_xmldoc.Clear();
}
