#include <node.h>
#include <v8.h>

#include <stdio.h>

#include <uv.h>

void thread_loop(uv_work_t* request);
void thread_done(uv_work_t* request, int status);
void on_progress(uv_async_t* handle, int status);

v8::Persistent<v8::Function> callback;

uv_loop_t* loop;
uv_async_t async;


v8::Handle<v8::Value> AddListener(const v8::Arguments& args)
{
  v8::Handle<v8::Function> cb = v8::Handle<v8::Function>::Cast(args[0]);

  callback = v8::Persistent<v8::Function>::New(cb);

  loop = uv_default_loop();
  // loop = uv_loop_new();

  uv_work_t request;

  uv_async_init(loop, &async, on_progress);
  uv_queue_work(loop, &request, thread_loop, thread_done);

  uv_run(loop, UV_RUN_DEFAULT);

  fprintf(stderr, "return undefined\n");

  return v8::Undefined();
}


void thread_loop(uv_work_t* request)
{
  fprintf(stderr, "Before sleep\n");
  Sleep(1000);
  uv_async_send(&async);
}

void thread_done(uv_work_t* request, int status)
{
  callback->Call(v8::Context::GetCurrent()->Global(), 0, 0);
  
  callback.Dispose();
  callback.Clear();

  uv_close((uv_handle_t*) &async, 0);
}

void on_progress(uv_async_t* handle, int status)
{
}

void init(v8::Handle<v8::Object> exports)
{
  exports->Set(v8::String::NewSymbol("addListener"), v8::FunctionTemplate::New(AddListener)->GetFunction());
}

NODE_MODULE(main, init)