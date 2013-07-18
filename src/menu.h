#ifndef __menu_h__
#define __menu_h__

#include <LiquidCrystal.h>
#include "util.h"

#define MENU_ROW_SUB 0
#define MENU_ROW_OPT 1
#define MENU_CLR_EOL "                "

class Menu;

class MenuItem {
public:
  MenuItem(const char* name) : 
  _name(name)
  {
    _level = 0;
    _next = NULL;
    _prev = NULL;
  }
  inline const char* getName() const {
    return _name;
  }
  byte getLevel() {
    return _level;
  }
  void setLevel(byte level) {
    _level = level;
  }
  void setMenu(Menu* m) {
    _menu = m;
  }
  boolean hasNext() {
    return (_next != NULL);
  }
  boolean hasPrev() {
    return (_prev != NULL);
  }
  MenuItem* get(byte n) {
    MenuItem* current = _first;
    for (byte i = 0; i < n; i++) {
      if (current->hasNext()) {
        current = current->getNext();
      } else {
        return NULL;
      }
    }
    return current;
  }
  MenuItem* getActive() {
    return _active;
  }
  MenuItem* getNext() {
    if (_next == NULL) {
      return getFirst();
    } 
    else {
      return _next;
    }
  }
  MenuItem* getPrev() {
    if (_prev == NULL) {
      return getLast();
    } 
    else {
      return _prev;
    }
  }
  MenuItem* getFirst() {
    MenuItem* current = this;
    while (current->hasPrev()) {
      current = current->getPrev();
    }
    return current;
  }
  MenuItem* getLast() {
    MenuItem* current = this;
    while (current->hasNext()) {
      current = current->getNext();
    }
    return current;
  }
  inline void setNext(MenuItem* n) {
    _next = n;
    n->setPrev(this);
  }
  inline void setPrev(MenuItem* p) {
    _prev = p;
  }

  void add(MenuItem* opt) {
    MenuItem* last;
    if (_first) {
      last = _first->getLast();
      last->setNext(opt);
      opt->setPrev(last);
    }
    else {
      _first = opt;
      _first->setNext(NULL);
      _first->setPrev(NULL);
      _active = _first;
    }
    opt->setLevel(_level + 1);
    opt->setMenu(_menu);
  }
  void next() {
    activate(_active->getNext());
  }
  void prev() {
    activate(_active->getPrev());
  }
  void activate(MenuItem *);
  void activate(byte n) {
    MenuItem *current = _first;
    for (byte i = 0; i < n; i++) {
      current = current->getNext();
    }
    _active = current;
  }

private:
  Menu*           _menu;
  byte            _level;
  // Sibling items
  MenuItem*        _next;
  MenuItem*        _prev;
  // Child items
  MenuItem*        _first;
  MenuItem*        _active;

protected:
  const char*     _name;
};

struct MenuItemEvent {
  const MenuItem item;
};

typedef void (*cb_change)(MenuItemEvent);
typedef void (*cb_select)(MenuItemEvent);

class Menu {
public:
  Menu(cb_change c, cb_select s) {
    _select = s;
    _change = c;
  }

  void add(MenuItem* sub) {
    MenuItem *last;
    if (_first) {
      last = _first->getLast();
      last->setNext(sub);
      sub->setPrev(last);
    } 
    else {
      _first = sub;
      _first->setNext(NULL);
      _first->setPrev(NULL);
      _active = _first;
    }
    sub->setMenu(this);
  }
  void next() {
    activate(_active->getNext());
  }
  void prev() {
    activate(_active->getPrev());
  }
  void activate(MenuItem* active) {
    _active = active;
    change(active);
  }
  MenuItem* getActive() {
    return _active;
  }
  void change(MenuItem *m) {
    MenuItemEvent event = { 
      *m     };
    (*_change)(event);
  }
  void select() {
    MenuItemEvent event = { *_active->getActive() }; 
    (*_select)(event);
  }

private:
  cb_select       _select;
  cb_change       _change;
  MenuItem*        _active;
  MenuItem*        _first;
};

void
MenuItem::activate(MenuItem* active) {
  _active = active;
  _menu->change(active);
}


bool operator==(MenuItem &a, char* b) {
  return strcmp(a.getName(), b) == 0;
}

bool operator==(MenuItem &a, MenuItem &b) {
  return strcmp(a.getName(), b.getName()) == 0;
}

#endif


