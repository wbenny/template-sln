#include <mutex>
#include <tuple>
#include <cstdio>
#include <type_traits>
#include <iostream>
#include <windows.h>

template <
  typename UnderlyingIntegerType,
  typename PointedType = void
>
class ptr_t;

template <
  typename UnderlyingIntegerType,
  typename PointedType
>
class ptr_t
{
  public:
    using pointed_type    = std::conditional_t<
                              std::is_pointer_v<PointedType>,
                              ptr_t<UnderlyingIntegerType, std::remove_pointer_t<PointedType>>,
                              PointedType
                            >;
    using underlying_type = UnderlyingIntegerType;
    using pointer         = pointed_type*;
    using reference       = pointed_type&;

    ptr_t()                                         noexcept = default;
    ptr_t(const ptr_t& other)                       noexcept = default;
    ptr_t(ptr_t&& other)                            noexcept = default;
    ptr_t& operator=(const ptr_t& other)            noexcept = default;
    ptr_t& operator=(ptr_t&& other)                 noexcept = default;
    ptr_t(const void* va)                           noexcept : value_{ UnderlyingIntegerType(uint64_t(va)) } {  }
    ptr_t(underlying_type va)                       noexcept : value_{ va } {                                   }

    ptr_t& operator= (underlying_type other)        noexcept { value_ = other; return *this;                    }

    ptr_t  operator+ (underlying_type other)  const noexcept { return ptr_t(value_ + other * sizeof(pointed_type));  }
    ptr_t& operator+=(underlying_type other)        noexcept { value_ += other * sizeof(pointed_type); return *this; }

    ptr_t  operator- (underlying_type other)  const noexcept { return ptr_t(value_ - other * sizeof(pointed_type));  }
    ptr_t& operator-=(underlying_type other)        noexcept { value_ -= other * sizeof(pointed_type); return *this; }

    bool  operator<  (ptr_t other)            const noexcept { return value_  < other.value_;        }
    bool  operator<= (ptr_t other)            const noexcept { return value_ <= other.value_;        }
    bool  operator>  (ptr_t other)            const noexcept { return value_  > other.value_;        }
    bool  operator>= (ptr_t other)            const noexcept { return value_ >= other.value_;        }
    bool  operator== (ptr_t other)            const noexcept { return value_ == other.value_;        }
    bool  operator!= (ptr_t other)            const noexcept { return value_ != other.value_;        }
    bool  operator!  (           )            const noexcept { return !value_;                       }

    ptr_t& operator++(   )                          noexcept {                   value_ += sizeof(value_); return *this; }
    ptr_t  operator++(int)                          noexcept { auto tmp = *this; value_ += sizeof(value_); return tmp;   }
    ptr_t& operator--(   )                          noexcept {                   value_ -= sizeof(value_); return *this; }
    ptr_t  operator--(int)                          noexcept { auto tmp = *this; value_ -= sizeof(value_); return tmp;   }

    reference operator*()                     const noexcept { return *reinterpret_cast<pointer>(value()); }
    pointer   operator->()                    const noexcept { return  reinterpret_cast<pointer>(value()); }

    auto  value()                             const noexcept { return value_;                        }
    auto  ptr()                               const noexcept { return (const void*)(value_);         }

  private:
    UnderlyingIntegerType value_;
};

template <
  typename UnderlyingIntegerType
>
class ptr_t<UnderlyingIntegerType, void>
{
  public:
    using underlying_type = UnderlyingIntegerType;

    ptr_t()                                         noexcept = default;
    ptr_t(const ptr_t& other)                       noexcept = default;
    ptr_t(ptr_t&& other)                            noexcept = default;
    ptr_t& operator=(const ptr_t& other)            noexcept = default;
    ptr_t& operator=(ptr_t&& other)                 noexcept = default;
    ptr_t(const void* va)                           noexcept : value_{ UnderlyingIntegerType(uint64_t(va)) } {  }
    ptr_t(underlying_type va)                       noexcept : value_{ va } {                                   }

    ptr_t& operator= (underlying_type other)        noexcept { value_ = other; return *this;                    }

    bool  operator<  (ptr_t other)            const noexcept { return value_  < other.value_;        }
    bool  operator<= (ptr_t other)            const noexcept { return value_ <= other.value_;        }
    bool  operator>  (ptr_t other)            const noexcept { return value_  > other.value_;        }
    bool  operator>= (ptr_t other)            const noexcept { return value_ >= other.value_;        }
    bool  operator== (ptr_t other)            const noexcept { return value_ == other.value_;        }
    bool  operator!= (ptr_t other)            const noexcept { return value_ != other.value_;        }
    bool  operator!  (           )            const noexcept { return !value_;                       }

    ptr_t& operator++(   )                          noexcept {                   value_ += sizeof(value_); return *this; }
    ptr_t  operator++(int)                          noexcept { auto tmp = *this; value_ += sizeof(value_); return tmp;   }
    ptr_t& operator--(   )                          noexcept {                   value_ -= sizeof(value_); return *this; }
    ptr_t  operator--(int)                          noexcept { auto tmp = *this; value_ -= sizeof(value_); return tmp;   }

    auto  value()                             const noexcept { return value_;                        }
    auto  ptr()                               const noexcept { return (const void*)(value_);         }

  private:
    UnderlyingIntegerType value_;
};

template <typename T = void> using ptr32_t = ptr_t<uint32_t, T>;
template <typename T = void> using ptr64_t = ptr_t<uint64_t, T>;

//////////////////////////////////////////////////////////////////////////

struct ptr_accessor_t
{
  static const ptr_accessor_t& select(int machine) noexcept;

  static const ptr_accessor_t x86;
  static const ptr_accessor_t x64;

  using functor_type = void* (*)(void* address);

  functor_type dereference;
  functor_type increment;
  functor_type decrement;
};

//////////////////////////////////////////////////////////////////////////

using uintptr32_t = uint32_t;
using uintptr64_t = uint64_t;

namespace detail {

  template <typename T>
  void* ptr_dereference(void* address) noexcept
  {
    static_assert(std::is_integral_v<T>);

    return reinterpret_cast<void*>(
      static_cast<uintptr_t>(
        static_cast<T>(
          reinterpret_cast<uintptr_t>(address)
          )));
  }

  template <typename T>
  void* ptr_increment(void* address) noexcept
  {
    static_assert(std::is_integral_v<T>);

    return reinterpret_cast<void*>(
      static_cast<uintptr_t>(
        static_cast<T>(
          reinterpret_cast<uintptr_t>(address)
          ) + sizeof(T)));
  }

  template <typename T>
  void* ptr_decrement(void* address) noexcept
  {
    static_assert(std::is_integral_v<T>);

    return reinterpret_cast<void*>(
      static_cast<uintptr_t>(
        static_cast<T>(
          reinterpret_cast<uintptr_t>(address)
          ) + sizeof(T)));
  }

  void* ptr_dereference_x86(void* address) noexcept
  {
    return ptr_dereference<uintptr32_t>(address);
  }

  void* ptr_dereference_x64(void* address) noexcept
  {
    return ptr_dereference<uintptr64_t>(address);
  }

  void* ptr_increment_x86(void* address) noexcept
  {
    return ptr_increment<uintptr32_t>(address);
  }

  void* ptr_increment_x64(void* address) noexcept
  {
    return ptr_increment<uintptr64_t>(address);
  }

  void* ptr_decrement_x86(void* address) noexcept
  {
    return ptr_decrement<uintptr32_t>(address);
  }

  void* ptr_decrement_x64(void* address) noexcept
  {
    return ptr_decrement<uintptr64_t>(address);
  }

}

const ptr_accessor_t ptr_accessor_t::x86 = { &detail::ptr_dereference_x86, &detail::ptr_increment_x86, &detail::ptr_decrement_x86 };
const ptr_accessor_t ptr_accessor_t::x64 = { &detail::ptr_dereference_x64, &detail::ptr_increment_x64, &detail::ptr_decrement_x64 };

const ptr_accessor_t& ptr_accessor_t::select(int machine) noexcept
{
  static constexpr ptr_accessor_t invalid_accessor = { nullptr, nullptr, nullptr };

  switch (machine)
  {
    case 1:
      return ptr_accessor_t::x64;

    case 2:
      return ptr_accessor_t::x64;

    default:
      return invalid_accessor;
  }
}


//////////////////////////////////////////////////////////////////////////

int main()
{
//   void* rp32_1 = VirtualAlloc((PVOID)0x10000, 16 * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
//   void* rp64_1 = VirtualAlloc((PVOID)0x20000, 16 * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
//
//   uint32_t* rp32t_1 = (uint32_t*)rp32_1;
//   for (uint32_t i = 0; i < 16; i++) rp32t_1[i] = i | 0xf0000000ul;
//
//   uint64_t* rp64t_1 = (uint64_t*)rp64_1;
//   for (uint64_t i = 0; i < 16; i++) rp64t_1[i] = i | 0xf000000000000000ull;
//
//   auto p32_1 = ptr32_t<void*>(rp32_1);
//   auto p64_1 = ptr64_t<void*>(rp64_1);
//
//   std::cout << std::hex;
//   std::cout << (*(p32_1 + 0)).value() << std::endl;
//   std::cout << (*(p32_1 + 1)).value() << std::endl;
//   std::cout << (*(p32_1 + 2)).value() << std::endl;
//
//   std::cout << (*(p64_1 + 0)).value() << std::endl;
//   std::cout << (*(p64_1 + 1)).value() << std::endl;
//   std::cout << (*(p64_1 + 2)).value() << std::endl;

}
