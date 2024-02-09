#pragma once

#define gettable(type, name) \
public: \
type name () { return this->_##name; } \
private: \
type _##name;


#define gettableProt(type, name) \
public: \
type name () { return this->_##name; } \
protected: \
type _##name;
