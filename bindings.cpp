#include <node_api.h>
#include <Api/eXaDrums.hpp>
#include <iostream>
#include <thread>
#include <assert.h>

using namespace std;
using namespace eXaDrumsApi;
using namespace Util;

#define NUM_OBJECTS_TO_REPORT 4
typedef enum { bird=0, cat, dog, car, bus, bicycle } SearchObjects;

namespace drumcontrol {

  typedef struct
  {
    int frame;
    SearchObjects obj;
  } ObjInfoThreadSafeFunctionDataEx_t;

  typedef struct
  {
    napi_async_work work_StreamSearch;
    napi_threadsafe_function tsfn_StreamSearch;
    int32_t MaxSearchTime;
  } AsyncStreamDataEx_t;

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

  void afterstart(int triggerId, float value) {
     printf("Event %d: %g \n", triggerId, value);
  }

  napi_value Init(napi_env env, napi_callback_info info) {
    // napi_status status;
 
    eXaDrumsApi::eXaDrums drumKit{"/root/.eXaDrums/Data/"};         
    
    error initError = drumKit.GetInitError();    
    if (initError.type != error_type_success) {
      napi_throw_error(env, "E_INIT", drumKit.GetInitError().message);
    } else {
//      thread func_thread(loop_thread, ref(drumKit));
//      if (func_thread.joinable()) func_thread.join(); 
      //loop_thread(drumKit);
    }

    return nullptr;    
  }

  // This function runs on the main thread after `ExecuteWork` exited.
  static void OnWorkComplete(napi_env env, napi_status status, void *data) {
    // Clean up the thread-safe function and the work item associated with this
    AsyncStreamDataEx_t *async_stream_data_ex = (AsyncStreamDataEx_t *)data;
    assert(napi_release_threadsafe_function(async_stream_data_ex->tsfn_StreamSearch, napi_tsfn_release) == napi_ok);
    assert(napi_delete_async_work(env, async_stream_data_ex->work_StreamSearch) == napi_ok);    
    free(data);
  }

  // This function runs on a worker thread. Then we cannot call JavaScript from
  // this thread, it can be called only from a native addon's main thread
  // except through the thread-safe function.
  static void ExecuteWork(napi_env env, void *data)
  {
    AsyncStreamDataEx_t *async_stream_data_ex = (AsyncStreamDataEx_t *)data;

    // called to indicate that a new thread will start making use of the thread-safe function
    // parameter: the asynchronous thread-safe JavaScript function (we crated by napi_create_threadsafe_function)
    assert(napi_acquire_threadsafe_function(async_stream_data_ex->tsfn_StreamSearch) == napi_ok);
    eXaDrumsApi::eXaDrums drumKit{"/root/.eXaDrums/Data/"};

    error initError = drumKit.GetInitError();    
    
    if (initError.type != error_type_success) {
      napi_throw_error(env, "E_INIT", drumKit.GetInitError().message);
    } else {
      // thread func_thread(loop_thread, ref(drumKit));
      // if (func_thread.joinable()) func_thread.join(); 
      // loop_thread(drumKit);
      drumKit.Start(afterstart);
    }

    while(true) {
      // This memory will be free after processing of the C callback
      // ObjInfoThreadSafeFunctionDataEx_t *pObjList = (ObjInfoThreadSafeFunctionDataEx_t *)malloc(NUM_OBJECTS_TO_REPORT * sizeof(ObjInfoThreadSafeFunctionDataEx_t));

      // Call the thread safe function, that can call JavaScritp callback to push data to JavaScript
      napi_call_threadsafe_function(async_stream_data_ex->tsfn_StreamSearch, pObjList, napi_tsfn_nonblocking)
    }

    // Indicate that this thread will make no further use of the thread-safe function.
    assert(napi_release_threadsafe_function(async_stream_data_ex->tsfn_StreamSearch, napi_tsfn_release) == napi_ok);
  }


  // This function is responsible for converting data coming in from the worker thread to
  // napi_value items that can be passed into JavaScript, and also call the JavaScript callback function.
  static void ThreadSafeCFunction4CallingJS(napi_env env, napi_value js_cb, void *context, void *data) {
    // This parameter is not used.
    (void)context;

    // data: the result what worker thread has created.
    // ObjInfoThreadSafeFunctionDataEx_t *pObjList = (ObjInfoThreadSafeFunctionDataEx_t *)data;
    int ObjectArraySize = NUM_OBJECTS_TO_REPORT;

    // env and js_cb may both be NULL if Node.js is in its cleanup phase, and
    // items are left over from earlier thread-safe calls from the worker thread.
    // When env is NULL, we simply skip over the call into Javascript and free the
    // items.
    if (env != NULL) {
      napi_value undefined;
      napi_value js_result_array;

      assert(napi_create_array_with_length(env, ObjectArraySize, &js_result_array) == napi_ok);

      for (int i = 0; i < ObjectArraySize; ++i) {
        napi_value js_obj;
        napi_value val_obj;
        //napi_value val_frame;
        //const char *obj_name = GetObjName((pObjList + i)->obj);
        napi_create_object(env, &js_obj);
        // assert(napi_create_string_utf8(env, obj_name, NAPI_AUTO_LENGTH, &val_obj) == napi_ok);
        //assert(napi_create_int32(env, (pObjList + i)->frame, &val_frame) == napi_ok);

        assert(napi_set_named_property(env, js_obj, "obj", val_obj) == napi_ok);
        // assert(napi_set_named_property(env, js_obj, "f", val_frame) == napi_ok);

        // Add the object to the array
        assert(napi_set_element(env, js_result_array, i, js_obj) == napi_ok);
      }

      // Retrieve the JavaScript `undefined` value so we can use it as the `this`
      // value of the JavaScript function call.
      assert(napi_get_undefined(env, &undefined) == napi_ok);

      // Call the JavaScript function and pass the N-API JavaScript value
      assert(napi_call_function(env, undefined, js_cb, 1, &js_result_array, NULL) == napi_ok);
    }

    // Free the item created by the worker thread.
    free(data);
  }

  napi_value CAsyncStreamSearch(napi_env env, napi_callback_info info) {
    napi_value js_cb;
    napi_value work_name;

    int32_t MaxSearchTime = 0;
    const size_t MaxArgExpected = 2;
    napi_value args[MaxArgExpected];
    size_t argc = sizeof(args) / sizeof(napi_value);
   
    AsyncStreamDataEx_t *async_stream_data_ex = NULL;
    assert(napi_get_cb_info(env, info, &argc, args, NULL, NULL) == napi_ok);

    if (argc != 2) napi_throw_error(env, "EINVAL", "AsyncStreamSearch: Argument count mismatch");

    // First argument: The amount of time in sec it is allowed to search
    assert(napi_get_value_int32(env, args[0], &MaxSearchTime) == napi_ok);
    js_cb = args[1]; // Second param, the JS callback function

    // This memory will be freed in OnWorkComplete function
    async_stream_data_ex = (AsyncStreamDataEx_t *)malloc(sizeof(*async_stream_data_ex));
    async_stream_data_ex->tsfn_StreamSearch = NULL;
    async_stream_data_ex->work_StreamSearch = NULL;
    async_stream_data_ex->MaxSearchTime = MaxSearchTime;

    // Specify a name to describe this asynchronous operation.
    assert(napi_create_string_utf8(env, "Thread-safe AsyncStreamSearch Work Item", NAPI_AUTO_LENGTH, &work_name) == napi_ok);

    // Create a thread-safe N-API callback function correspond to the C/C++ callback function
    assert(napi_create_threadsafe_function(env,
        js_cb, NULL, work_name, 0, 1, NULL, NULL, NULL,
        ThreadSafeCFunction4CallingJS, // the C/C++ callback function
        // out: the asynchronous thread-safe JavaScript function
        &(async_stream_data_ex->tsfn_StreamSearch)) == napi_ok);

    // Create an async work item, that can be deployed in the node.js event queue
    // worker thread access to the above-created thread-safe function.
    assert(napi_create_async_work( env, NULL,
        work_name,
        ExecuteWork,
        OnWorkComplete,
        async_stream_data_ex,
        // OUT: THE handle to the async work item
        &(async_stream_data_ex->work_StreamSearch)) == napi_ok);

    // Queue the work item for execution.
    assert(napi_queue_async_work(env, async_stream_data_ex->work_StreamSearch) == napi_ok);

    // This causes `undefined` to be returned to JavaScript.
    return NULL;
  }

  napi_value init(napi_env env, napi_value exports) {
    napi_status status;
    napi_value fn;
        
    status = napi_create_function(env, "Init", NAPI_AUTO_LENGTH, CAsyncStreamSearch, NULL, &fn);

    //status = napi_create_function(env, nullptr, 0, Init, nullptr, &fn);
    // status = napi_create_threadsafe_function(env, Init, nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr, &fn);

    if (status != napi_ok) return nullptr;

    status = napi_set_named_property(env, exports, "init", fn);
    if (status != napi_ok) return nullptr;
    return exports;
  }

  NAPI_MODULE(NODE_GYP_MODULE_NAME, init)

}
