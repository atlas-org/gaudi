// $Id: ListItem.h,v 1.2 2007/01/17 14:17:08 hmd Exp $
#ifndef GAUDIKERNEL_LISTITEM_H
#define GAUDIKERNEL_LISTITEM_H
/** Definition of a small helper class to interprete Algorithm names
*/

#include <string>

class ListItem    {
  std::string m_type, m_name;
  void init(const std::string& itm, const std::string& deftyp)    {
    int slash_pos = itm.find_first_of("/");
    m_name = (slash_pos > 0) ? itm.substr( slash_pos + 1) : itm;
    m_type = (slash_pos > 0) ? itm.substr( 0, slash_pos ) : deftyp;
  }
public:
  ListItem(const std::string& itm)    {
    init(itm, itm);
  }
  ListItem(const std::string& itm, const std::string& deftyp)    {
    init(itm, deftyp);
  }
  const std::string& type()   const   {
    return m_type;
  }
  const std::string& name()   const   {
    return m_name;
  }
};
#endif // GAUDIKERNEL_LISTITEM_H
