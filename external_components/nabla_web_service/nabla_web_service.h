// Central root navigation; leaf renderers own only their namespaced routes.
#pragma once
#include "esphome/core/component.h"
#include "esphome/components/web_server_base/web_server_base.h"
namespace esphome::nabla_web_service {
class Service:public Component,public AsyncWebHandler {
 public:
 void set_mode(int mode){mode_=mode;}
 float get_setup_priority() const override{return setup_priority::WIFI+3;}
 void setup() override {auto *base=web_server_base::global_web_server_base;base->init();base->add_handler(this);}
 bool canHandle(AsyncWebServerRequest *r)const override{return r->url()=="/";}
 void handleRequest(AsyncWebServerRequest *r)override{
  if(r->method()!=HTTP_GET){r->send(400);return;}
  if(mode_!=2){r->redirect(mode_==0?"/nabla":"/mirror");return;}
  r->send(200,"text/html; charset=utf-8",R"PAGE(<!doctype html><html lang="es"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Nabla</title><style>body{background:#08090b;color:#eee;font:20px monospace;margin:0;padding:24px}nav{display:flex;gap:20px;flex-wrap:wrap}a{border:1px solid #404040;border-radius:12px;padding:32px;color:inherit;text-decoration:none}a:focus-visible,a:hover{outline:2px solid white}</style><h1>▽ Nabla</h1><nav><a href="/nabla">Menú web</a><a href="/mirror">Pantalla del dispositivo</a></nav></html>)PAGE");
 }
 protected:int mode_=0;
};
}
