#ifndef FLAGS_H
#define FLAGS_H

// #include "Types.h"
#include <sys/types.h>
#include <stddef.h>
#include <limits.h>

class Flag
{
public:
    inline Flag(int value) : i(value) {}
    inline operator int() const { return i; }
    inline Flag(long value) : i(int(value)) {}
    // inline Flag(ulong value) : i(int(long(value))) {}
    inline Flag(uint value) : i(int(value)) {}
    inline Flag(short value) : i(int(value)) {}
    // inline Flag(ushort value) : i(int(uint(value))) {}
    inline operator uint() const { return uint(i); }

private:
    int i;
};

template<typename Enum>
class Flags
{
public:
    typedef int Int;

    inline Flags() : i(0) {}
    inline Flags(Enum flags) : i(int(flags)) {}
    inline Flags(int flag) : i(flag) {}
    // inline Flags(uint flag): i (int(flag)){ }
    inline Flags(Flag flag) : i(flag) {}

    inline Flags &operator=(Flags other) { i = other.i; return *this; }
    inline Flags &operator=(int flag) { i = flag; return *this; }
    inline Flags &operator=(uint flag) { i = int(flag); return *this; }
    inline Flags &operator&=(int mask) { i &= mask; return *this; }
    inline Flags &operator&=(uint mask) { i &= mask; return *this; }
    inline Flags &operator&=(Enum mask) { i &= int(mask); return *this; }
    inline Flags &operator|=(Flags other) { i |= other.i; return *this; }
    inline Flags &operator|=(Enum other) { i |= int(other); return *this; }
    inline Flags &operator^=(Flags other) { i ^= other.i; return *this; }
    inline Flags &operator^=(Enum other) { i ^= int(other); return *this; }

    inline operator int() const { return i; }

    inline Flags operator|(Flags other) const { return Flags(Flag(i | other.i)); }
    inline Flags operator|(Enum other) const { return Flags(Flag(i | int(other))); }
    inline Flags operator^(Flags other) const { return Flags(Flag(i ^ other.i)); }
    inline Flags operator^(Enum other) const { return Flags(Flag(i ^ int(other))); }
    inline Flags operator&(int mask) const { return Flags(Flag(i & mask)); }
    inline Flags operator&(uint mask) const { return Flags(Flag(i & mask)); }
    inline Flags operator&(Enum other) const { return Flags(Flag(i & int(other))); }
    inline Flags operator~() const { return Flags(Flag(~i)); }

    inline bool operator!() const { return !i; }

    inline bool testFlag(Enum flag) const { return (i & int(flag)) == int(flag) && (int(flag) != 0 || i == int(flag) ); }
    inline Flags &setFlag(Enum flag, bool on = true) { return on ? (*this |= flag) : (*this &= ~int(flag)); }

private:
    int i;
};

#define DECLARE_FLAGS(FlagsTp, Enum) typedef Flags<Enum> FlagsTp;
#define DECLARE_INCOMPATIBLE_FLAGS(Flags) \
//inline IncompatibleFlag operator|(Flags::enum_type f1, int f2) \
//{ return IncompatibleFlag(int(f1) | f2); }

#define DECLARE_OPERATORS_FOR_FLAGS(Flags) \
//inline Flags<Flags::enum_type> operator|(Flags::enum_type f1, Flags::enum_type f2) \
//{ return Flags<Flags::enum_type>(f1) | f2; } \
//inline Flags<Flags::enum_type> operator|(Flags::enum_type f1, Flags<Flags::enum_type> f2) \
//{ return f2 | f1; } \
//DECLARE_INCOMPATIBLE_FLAGS(Flags)
#endif // FLAGS_H
