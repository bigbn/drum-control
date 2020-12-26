#include <node_api.h>
#include <Api/eXaDrums.hpp>
#include <iostream>

using namespace std;
using namespace eXaDrumsApi;
using namespace Util;

namespace drumcontrol {

  /*
  * N-API value to std::string
  */
  std::string getString(napi_env env, napi_value value) {
    napi_status status;
    size_t str_size;
    size_t str_size_read;
    status = napi_get_value_string_utf8(env, value, NULL, 0, &str_size);
    if (status != napi_ok) return nullptr;
    char * buf;
    buf = (char*)calloc(str_size + 1, sizeof(char));
    str_size = str_size + 1;
    napi_get_value_string_utf8(env, value, buf, str_size, &str_size_read);
    std::string package_name(buf);
    return package_name;
  }


  napi_value Init(napi_env env, napi_callback_info info) {
    napi_status status;
 
  	eXaDrumsApi::eXaDrums drumKit{"/home/bigbn/drums"};         
    
    error initError = drumKit.GetInitError();    
    if (initError.type != error_type_success) {
      napi_throw_error(env, "E_INIT", drumKit.GetInitError().message);
    } else {
      drumKit.Start();
    }      

    return nullptr;    
  }

  napi_value init(napi_env env, napi_value exports) {
    napi_status status;
    napi_value fn;
    
    status = napi_create_function(env, nullptr, 0, Init, nullptr, &fn);
    if (status != napi_ok) return nullptr;

    status = napi_set_named_property(env, exports, "init", fn);
    if (status != napi_ok) return nullptr;
    return exports;
  }

  NAPI_MODULE(NODE_GYP_MODULE_NAME, init)

}