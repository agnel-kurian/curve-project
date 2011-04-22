#pragma once

namespace App0 {

  class app0_exception
  {
    TCHAR* msg;

  public:
    app0_exception(TCHAR* msg);
    virtual ~app0_exception(void);

    TCHAR* message();
  };

}