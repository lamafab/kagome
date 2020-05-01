# Code Review of Kagome

## Project details

Project: Kagome - C++17 implementation of Polkadot Runtime Environment  
URL: https://github.com/soramitsu/kagome  
Review based on commit: `988bc6d`  

Code review issue tracker: https://github.com/w3f/polkadot-spec/issues/172

## Author

Author: Fabio Lama  
Start date: 01. May 2020  
End date: ...  

## Components

- State Transition
- SCALE
- Host API

## Implementation

### State Transition

...

### SCALE

**Root Code path**: `core/scale`

#### Conformance summary

The Kagome implementation fulfills the following requirements for SCALE encoding:

- [ ] - Encoding/Decoding of fixed-width integers
  - [ ] - (un-)signed 8-bit integers
  - [ ] - (un-)signed 16-bit integers
  - [ ] - (un-)signed 32-bit integers
  - [ ] - (un-)signed 64-bit integers
  - [ ] - (un-)signed 128-bit integers
- [ ] - Encoding/Decoding of compact integers
  - [ ] - single-byte mode
  - [ ] - two-byte mode
  - [ ] - four-byte mode
  - [ ] - big integer mode
- [ ] - Encoding/Decoding of booleans
- [ ] - Encoding/Decoding of optional values ("Option")
- [ ] - Encoding/Decoding of success/failure indicators ("Result")
- [ ] - Encoding/Decoding of Vectors (lists, series, arrays)
- [ ] - Encoding/Decoding of tuples
- [ ] - Encoding/Decoding of data structures
- [ ] - Encoding/Decoding of enumerations

#### Public functions

**Code path**: `core/scale/scale.hpp`  
**Namespace**: `kagome::scale`
**Description**: Convenience functions for encoding/decoding primitive types to stream.

**Relevant interfaces**:
```cpp
outcome::result<std::vector<uint8_t>> encode(...)
outcome::result<T> decode(...)
```

**Code path**: `core/scale/scale_encoder_stream.hpp`  
**Namespace**: `kagome::scale`  
**Description**: The Class `ScaleEncoderStream` is the primary way of encoding data to SCALE. The class can be overloaded with the `<<` operator and return the result when calling the `.data()` method.

**Relevant interfaces**:
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

**Code path**: `core/scale/scale_decoder_stream.hpp`  
**Namespace**: `kagome::scale`  
**Description**: The Class `ScaleDecoderStream` is the primary way of decoding data from SCALE. The class can decode with the `>>` operator and assign directly to variables.

**Relevant interfaces**:
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

#### Internal mechanism

### Host API

...
