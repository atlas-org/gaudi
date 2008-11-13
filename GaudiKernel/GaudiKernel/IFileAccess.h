// $Id: IFileAccess.h,v 1.2 2008/06/12 15:20:27 marcocle Exp $
#ifndef GaudiKernel_IFileAccess_H
#define GaudiKernel_IFileAccess_H

// Include Files
#include "GaudiKernel/IInterface.h"
#include <string>
#include <istream>
#include <memory>
#include <vector>

// Forward declarations

static const InterfaceID IID_IFileAccess("IFileAccess", 1 , 0); 

/** @class IFileAccess GaudiKernel/IFileAccess GaudiKernel/IFileAccess.h
 *
 * Abstract interface for a service or tool implementing a read access to files.
 * 
 * @author Marco Clemencic
 * @date   2008-01-18
 */
class IFileAccess : virtual public IInterface {
  public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IFileAccess; }

  virtual ~IFileAccess(); //< Virtual destructor.
  
  /// Find the URL and returns an auto_ptr to an input stream interface of an
  /// object that can be used to read from the file the URL is pointing to.
  /// Returns an empty pointer if the URL cannot be resolved.
  virtual std::auto_ptr<std::istream> open(const std::string &url) = 0;

  /// Protocols supported by the instance.
  virtual const std::vector<std::string> &protocols() const = 0;
  
};

#endif // GaudiKernel_IFileAccess_H
