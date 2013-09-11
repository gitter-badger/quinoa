//******************************************************************************
/*!
  \file      src/Base/Printer.h
  \author    J. Bakosi
  \date      Wed Sep 11 17:14:45 2013
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Printer
  \details   Printer
*/
//******************************************************************************
#ifndef Printer_h
#define Printer_h

#include <string>

#include <boost/format.hpp>

namespace quinoa {

//! Printer base
class Printer {

  public:
    //! Constructor
    explicit Printer() noexcept = default;

    //! Destructor
    virtual ~Printer() noexcept = default;

    //! Print title
    void title(const std::string& title) const;

    //! Print section header: title
    void section(const std::string& title) const {
      using boost::format;
      using boost::io::group;

      std::cout << format("\n * %1%:\n") % title;
      std::cout << format(" %1%\n") % std::string(title.size()+3,'-');
    }
    //! Print section header: title : value
    template<typename T>
    void section(const std::string& name, const T& value) const {
      using boost::format;
      using boost::io::group;

      std::cout << format("\n * %1%: %2%\n") % name % value;
      std::cout << format(" %1%\n") %
                   std::string(name.size() + value.size() + 4, '-');
    }

    //! Print subsection header: title
    void subsection(const std::string& title) const {
      using boost::format;
      using boost::io::group;

      std::cout << format("\n   - %1%:\n") % title;
    }

    //! Print item: name
    void item(const std::string& name) const {
       using boost::format;
       using boost::io::group;

       std::cout << format("%35s\n") % name;
    }
    //! Print item: name : value
    template<typename T>
    void item(const std::string& name, const T& value) const {
       using boost::format;
       using boost::io::group;

       std::cout << format("%30s : %s\n") % name % value;
    }

  private:
    //! Don't permit copy constructor
    Printer(const Printer&) = delete;
    //! Don't permit copy assigment
    Printer& operator=(const Printer&) = delete;
    //! Don't permit move constructor
    Printer(Printer&&) = delete;
    //! Don't permit move assigment
    Printer& operator=(Printer&&) = delete;
};

} // namespace quinoa

#endif // Printer_h