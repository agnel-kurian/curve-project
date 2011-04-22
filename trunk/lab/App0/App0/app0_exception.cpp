#include "StdAfx.h"
#include "app0_exception.h"

namespace App0 {

  app0_exception::app0_exception(TCHAR* msg) : msg(msg)
  {
  }


  app0_exception::~app0_exception(void)
  {
  }

  TCHAR* app0_exception::message(){
    return msg;
  }
}