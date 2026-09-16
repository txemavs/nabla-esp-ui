#pragma once
#include <array>
#include <algorithm>
#include <cstdint>
#include <string>
namespace nabla_forms {
enum class Kind { NUMBER, CHOICE, TOGGLE };
struct Field {
  const char *key, *label;
  Kind kind;
  int minimum, maximum, step, initial;
  std::array<const char *,8> options;
};
extern const Field fields[];
extern const int field_count;
// Field acceptance edits a draft. Only confirmed Save commits retained values.
struct Session {
  const Field *spec=nullptr;
  int count=0;
  std::array<int,8> saved{}, draft{};
  bool active=false;
  uint32_t commits=0;
  void configure(const Field *data, int size) {
    spec=data;count=std::max(0,std::min(size,8));
    for(int i=0;i<count;i++)saved[i]=spec[i].initial;
    active=false;
  }
  void begin() {draft=saved;active=true;}
  bool valid() const {
    for(int i=0;i<count;i++){
      const auto &f=spec[i];
      if(draft[i]<f.minimum || draft[i]>f.maximum || f.step<=0 ||
         (int64_t(draft[i])-f.minimum)%f.step) return false;
    }
    return true;
  }
  bool commit() {
    if(!active || !valid())return false;
    saved=draft;active=false;++commits;return true;
  }
  void cancel() {draft=saved;active=false;}
};
enum class FormView { BROWSE, NUMBER, CHOICE, CONFIRM, SAVED, INVALID };
struct Controller {
  Session session;
  FormView view=FormView::BROWSE;
  int focus=0, top=0, field=0, value=0;
  const char *const *words=nullptr;
  void begin() {session.begin();view=FormView::BROWSE;focus=top=0;}
  void discard() {session.cancel();view=FormView::BROWSE;focus=top=0;}
  int total() const {
    switch(view){
      case FormView::NUMBER:return 4;
      case FormView::CHOICE:return session.spec[field].maximum+2;
      case FormView::CONFIRM:return 2;
      case FormView::SAVED:case FormView::INVALID:return 1;
      default:return session.count+2;
    }
  }
  void move(int delta) {
    int n=total();focus=(focus+delta%n+n)%n;
    if(view==FormView::CHOICE && focus<=session.spec[field].maximum)value=focus;
  }
  void adjust(int delta) {
    if(view!=FormView::NUMBER && view!=FormView::CHOICE)return;
    const auto &f=session.spec[field];
    value=static_cast<int>(std::clamp(int64_t(value)+int64_t(delta)*f.step,
                                    int64_t(f.minimum),int64_t(f.maximum)));
    if(view==FormView::CHOICE)focus=value;
  }
  void restore() {view=FormView::BROWSE;focus=field;top=0;}
  bool back() {
    if(view==FormView::NUMBER || view==FormView::CHOICE){restore();return false;}
    if(view==FormView::CONFIRM || view==FormView::INVALID){
      view=FormView::BROWSE;focus=session.count;top=0;return false;
    }
    discard();return true;
  }
  bool activate() {
    switch(view){
      case FormView::NUMBER:
        if(focus<2)adjust(focus?1:-1);
        else {if(focus==2)session.draft[field]=value;restore();}
        return false;
      case FormView::CHOICE:
        if(focus<=session.spec[field].maximum)session.draft[field]=focus;
        restore();return false;
      case FormView::CONFIRM:
        if(focus==0){view=session.commit()?FormView::SAVED:FormView::INVALID;focus=top=0;}
        else back();
        return false;
      case FormView::SAVED:return back();
      case FormView::INVALID:back();return false;
      default:break;
    }
    if(focus==session.count+1)return back();
    if(focus==session.count){view=FormView::CONFIRM;focus=1;top=0;return false;}
    field=focus;value=session.draft[field];
    const auto &f=session.spec[field];
    if(f.kind==Kind::TOGGLE){session.draft[field]=!value;return false;}
    view=f.kind==Kind::NUMBER?FormView::NUMBER:FormView::CHOICE;
    focus=f.kind==Kind::NUMBER?1:value;top=0;return false;
  }
  std::string display_value(int index,int n) const {
    const auto &f=session.spec[index];
    if(f.kind==Kind::NUMBER)return std::to_string(n);
    if(f.kind==Kind::TOGGLE)return words[n?7:8];
    return f.options[n];
  }
  std::string title() const {
    if(view==FormView::NUMBER || view==FormView::CHOICE)
      return std::string(session.spec[field].label)+": "+display_value(field,value);
    return words[view==FormView::CONFIRM?4:view==FormView::SAVED?5:view==FormView::INVALID?6:0];
  }
  std::string row(int index) const {
    if(view==FormView::NUMBER)return index<2?(index==0?"-  ":"+  ")+std::to_string(session.spec[field].step):words[index==2?3:2];
    if(view==FormView::CHOICE)return index<=session.spec[field].maximum?session.spec[field].options[index]:words[2];
    if(view==FormView::CONFIRM)return words[index==0?1:2];
    if(view==FormView::SAVED || view==FormView::INVALID)return words[9];
    if(index>=session.count)return words[index==session.count?1:2];
    return std::string(session.spec[index].label)+": "+display_value(index,session.draft[index]);
  }
};
}
