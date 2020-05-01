# Code Review of Kagome

## Project details

Project:    Kagome - C++17 implementation of Polkadot Runtime Environment  
URL:        https://github.com/soramitsu/kagome  

Code review issue tracker: https://github.com/w3f/polkadot-spec/issues/172
Review based on commit: `988bc6d`  

## Author

Author:     Fabio Lama  
Start date: 01. May 2020  
End date:   ...  

## Components

- State Transition
- SCALE Codec
- Host API

## Conformance Summary

### SCALE Codec

The Kagome implementation fulfills the following requirements for SCALE encoding:

- [ ] Encoding/Decoding of fixed-width integers
  - [ ] (un-)signed 8-bit integers
  - [ ] (un-)signed 16-bit integers
  - [ ] (un-)signed 32-bit integers
  - [ ] (un-)signed 64-bit integers
  - [ ] (un-)signed 128-bit integers
- [ ] Encoding/Decoding of compact integers
  - [ ] single-byte mode
  - [ ] two-byte mode
  - [ ] four-byte mode
  - [ ] big integer mode
- [ ] Encoding/Decoding of booleans
- [ ] Encoding/Decoding of optional values ("Option")
- [ ] Encoding/Decoding special case of optional boolean type ("Option")
- [ ] Encoding/Decoding of success/failure indicators ("Result")
- [ ] Encoding/Decoding of Vectors (lists, series, arrays)
- [ ] Encoding/Decoding of tuples
- [ ] Encoding/Decoding of data structures
- [ ] Encoding/Decoding of enumerations

## Component: State Transition

...

## Component: SCALE Codec

Root Code path: `core/scale`

### Relevant types

Code path:      `core/scale/scale.hpp`  
Namespace:      `kagome::scale`
Description:    Convenience functions for encoding/decoding primitive types to stream.
Code:

```cpp
outcome::result<std::vector<uint8_t>> encode(...)
outcome::result<T> decode(...)
```

Code path:      `core/scale/scale_encoder_stream.hpp`  
Namespace:      `kagome::scale`  
Description:    The Class `ScaleEncoderStream` is the primary way of encoding data to SCALE. The class can be overloaded with the `<<` operator and return the result when calling the `.data()` method.  
Code:

```cpp
class ScaleEncoderStream {
    public:
        // Return vector of bytes containing encoded data.
        std::vector<uint8_t> data() const;
        // Encodes pair of values
        ScaleEncoderStream &operator<<(const std::pair<F, S> &p) { ... }
        // Encode variant value
        ScaleEncoderStream &operator<<(const boost::variant<T...> &v) { ... }
        // Encode collection of items of the same type
        ScaleEncoderStream &operator<<(const std::vector<T> &c) { ... }
        // Encode optional value
        ScaleEncoderStream &operator<<(const boost::optional<T> &v) { ... }
        // Appends sequence of bytes
        ScaleEncoderStream &operator<<(const gsl::span<T> &v) { ... }
        // Scale-encodes array of items
        ScaleEncoderStream &operator<<(const std::array<T, size> &a) { ... }
        // Encodes unsigned 256-bit integer
        ScaleEncoderStream &operator<<(const boost::multiprecision::uint256_t &i) { ... }
        // Encodes reference wrapper of a type
        ScaleEncoderStream &operator<<(const std::reference_wrapper<T> &v) { ... }
        // Encodes a string view
        ScaleEncoderStream &operator<<(std::string_view sv) { ... }
        // Encodes any integral type including bool
        template <typename T,
                typename I = std::decay_t<T>,
                typename = std::enable_if_t<std::is_integral<I>::value>>
        ScaleEncoderStream &operator<<(T &&v) { ... }
        // Encodes value as compact integer
        ScaleEncoderStream &operator<<(const CompactInteger &v);
}
```

Code path:      `core/scale/scale_decoder_stream.hpp`  
Namespace:      `kagome::scale`  
Description:    The Class `ScaleDecoderStream` is the primary way of decoding data from SCALE. The class can decode with the `>>` operator and assign directly to variables.  
Code:

```cpp
class ScaleDecoderStream {
    public:
        // Decode pair of values
        template <class F, class S>
        ScaleDecoderStream &operator>>(std::pair<F, S> &p) { ... }
        // Decode variant value
        template <class... T>
        ScaleDecoderStream &operator>>(boost::variant<T...> &v) { ... }
        // Decode any integral type including bool
        template <typename T,
                typename I = std::decay_t<T>,
                typename = std::enable_if_t<std::is_integral<I>::value>>
        ScaleDecoderStream &operator>>(T &v) { ... }
        // Decode any optional value
        template <class T>
        ScaleDecoderStream &operator>>(boost::optional<T> &v) { ... }
        // Decode compact integer value
        ScaleDecoderStream &operator>>(CompactInteger &v);
        // Decode collection of items
        template <class T>
        ScaleDecoderStream &operator>>(std::vector<T> &v) { ... }
        // Decode array of items
        template <typename T, size_t size>
        ScaleDecoderStream &operator>>(std::array<T, size> &a) { ... }
        // Decode unsigned 256-bit integer
        ScaleDecoderStream &operator>>(boost::multiprecision::uint256_t &i) { ... }
        // Decode string
        ScaleDecoderStream &operator>>(std::string &v);
}
```

### Internal mechanism

#### Encode compact integers

|             |                                       |
| ----------- | ------------------------------------- |
|Code path:   | `core/scale/scale_encoder_stream.cpp` |
|Function:    | `encodeCompactInteger(..)`            |
|Conformance: | **compliant**                         |

1. The function identifies the integer size and calls the correspond function internally. The function returns the mode of the compact integer.

```cpp
inline void encodeFirstCategory(uint8_t value, ScaleEncoderStream &out) {
    // only values from [0, kMinUint16) can be put here
    out << static_cast<uint8_t>(value << 2u);
}

inline void encodeSecondCategory(uint16_t value, ScaleEncoderStream &out) {
    // only values from [kMinUint16, kMinUint32) can be put here
    auto v = value;
    v <<= 2u;  // v *= 4
    v += 1u;   // set 0b01 flag
    auto minor_byte = static_cast<uint8_t>(v & 0xFFu);
    v >>= 8u;
    auto major_byte = static_cast<uint8_t>(v & 0xFFu);

    out << minor_byte << major_byte;
}

inline void encodeThirdCategory(uint32_t value, ScaleEncoderStream &out) {
    // only values from [kMinUint32, kMinBigInteger) can be put here
    uint32_t v = (value << 2u) + 2;
    scale::detail::encodeInteger<uint32_t>(v, out);
}
```

2. Count the number of bytes required to represent value.
3. TODO: Clarify `uint8_t header = (bigIntLength - 4) * 4 + 3;`
4. Shift the value accordingly, prefixed by the compact integer mode indicator.
5. Write result to buffer.

#### Encode optional booleans

|             |                                       |
| ----------- | ------------------------------------- |
|Code path:   | `core/scale/scale_encoder_stream.cpp` |
|Function:    | `encodeOptionalBool(..)`              |
|Conformance: | **compliant**                         |

1. The function checks if the variable contains a value.
    - If it does not, encode as `0x00`.
    - If it does and boolean is `false`, encode as `0x01`.
    - If it does and boolean is `true`, encode as `0x02`.
2. Write result to buffer.

```cpp
auto result = OptionalBool::TRUE;

if (!v.has_value()) {
    result = OptionalBool::NONE;
} else if (!*v) {
    result = OptionalBool::FALSE;
}

return putByte(static_cast<uint8_t>(result));
```

#### Encode fixed-width integer

|             |                                             |
| ----------- | ------------------------------------------- |
|Code path:   | `core/scale/detail/fixed_witdh_integer.hpp` |
|Function:    | `encodeInteger(..)`                         |
|Conformance: | **compliant**                               |

1. Converts the fixed-width integer to little-endian representation.

#### Encode Vectors (lists, series, arrays)

|             |                                       |
| ----------- | ------------------------------------- |
|Code path:   | `core/scale/scale_encoder_stream.hpp` |
|Function:    | `encodeCollection(..)`                |
|Conformance: | **compliant**                         |

1. Writes the size (amount of elements) of the collection to the buffer (by using a compact integer type).
2. Writes each element to the buffer.

```cpp
template <class It>
ScaleEncoderStream &encodeCollection(const CompactInteger &size,
        It &&begin,
        It &&end)
{
    *this << size;
    for (auto &&it = begin; it != end; ++it) {
    *this << *it;
    }
    return *this;
}
```

TODO: Does each element get encoded accordingly?

#### Encode boolean

|             |                                       |
| ----------- | ------------------------------------- |
|Code path:   | `core/scale/scale_encoder_stream.hpp` |
|Function:    | `ScaleEncoderStream &operator<<(..)`  |
|Conformance: | **compliant**                         |

1. Checks if the value is `true`.
    - If it is, write `0x01` to buffer.
    - If it is not, write `0x00` to buffer.

```cpp
if constexpr (std::is_same<I, bool>::value) {
    uint8_t byte = (v ? 1u : 0u);
    return putByte(byte);
}
```

### Component: Host API

...
