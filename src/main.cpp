// #include <node.h>
// #include <v8.h>

// #include <stdio.h>

// #include <uv.h>

// void thread_loop(uv_work_t* request);
// void thread_done(uv_work_t* request, int status);
// void on_progress(uv_async_t* handle, int status);

// v8::Persistent<v8::Function> callback;

// uv_loop_t* loop;
// uv_async_t async;


// v8::Handle<v8::Value> AddListener(const v8::Arguments& args)
// {
//   v8::Handle<v8::Function> cb = v8::Handle<v8::Function>::Cast(args[0]);

//   callback = v8::Persistent<v8::Function>::New(cb);

//   loop = uv_default_loop();
//   // loop = uv_loop_new();

//   uv_work_t request;

//   uv_async_init(loop, &async, on_progress);
//   uv_queue_work(loop, &request, thread_loop, thread_done);

//   uv_run(loop, UV_RUN_DEFAULT);

//   fprintf(stderr, "return undefined\n");

//   return v8::Undefined();
// }


// void thread_loop(uv_work_t* request)
// {
//   fprintf(stderr, "Before sleep\n");
//   Sleep(1000);
//   uv_async_send(&async);
// }

// void thread_done(uv_work_t* request, int status)
// {
//   callback->Call(v8::Context::GetCurrent()->Global(), 0, 0);
  
//   callback.Dispose();
//   callback.Clear();

//   uv_close((uv_handle_t*) &async, 0);
// }

// void on_progress(uv_async_t* handle, int status)
// {
// }

// void init(v8::Handle<v8::Object> exports)
// {
//   exports->Set(v8::String::NewSymbol("addListener"), v8::FunctionTemplate::New(AddListener)->GetFunction());
// }

// NODE_MODULE(main, init)


#include <node.h>
#include <v8.h>

#include <uv.h>

#include <stdio.h>

using namespace v8;



void DoAsync(uv_work_t*);
void ProgressUpdate(uv_async_t*, int);
void AfterAsync(uv_work_t*);

void writeData();



// Global vars
Persistent<Function> persist;
uv_async_t async;
uv_rwlock_t lock;



// structs
struct async_req {
  uv_work_t req;
  Persistent<Function> callback;
};

struct myo_data {
  float orientation_x, orientation_y, orientation_z;
} data;






void DoAsync(uv_work_t* r)
{
  int i = 10;

  while (i--)
  {
    Sleep(1000);

    writeData();

    uv_async_send(&async);    
  }
}

void ProgressUpdate(uv_async_t* handle, int status)
{
  Local<Object> obj = Object::New();
  uv_rwlock_wrlock(&lock);

  obj->Set(String::NewSymbol("orientation_x"), Number::New(data.orientation_x));

  uv_rwlock_wrunlock(&lock);

  const unsigned argc = 1;
  Local<Value> argv[argc] = { obj };

  persist->Call(Context::GetCurrent()->Global(), argc, argv);
}

void AfterAsync(uv_work_t* r)
{ 
  fprintf(stderr, "AfterAsync\n");

  uv_close((uv_handle_t*) &async, 0);
}




void writeData()
{
  uv_rwlock_wrlock(&lock);

  data.orientation_x++;
  data.orientation_y++;
  data.orientation_z++;

  uv_rwlock_wrunlock(&lock);
}








// Exports
v8::Handle<v8::Value> AddListener(const v8::Arguments& args)
{
  Handle<Function> ftpl = Handle<Function>::Cast(args[0]);

  async_req* req = new async_req;
  persist = Persistent<Function>::New(ftpl);

  // setup our thread lock
  uv_rwlock_init(&lock);

  // setup our async handler, injects into MainEvent loop
  uv_async_init(uv_default_loop(), &async, ProgressUpdate);
  // setup working thread
  uv_queue_work(uv_default_loop(),
                &req->req,
                DoAsync,
                (uv_after_work_cb)AfterAsync);

  return Undefined();
}


// setup module
void init(v8::Handle<v8::Object> exports)
{
  exports->Set(v8::String::NewSymbol("addListener"), v8::FunctionTemplate::New(AddListener)->GetFunction());
}

NODE_MODULE(main, init)
