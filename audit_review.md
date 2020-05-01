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

Root code path: `core/scale`

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

Code path: `core/scale/scale.hpp`  
Namespace: `kagome::scale`
Description: Convenience functions for encoding/decoding primitive types to stream.

Function interface(s):
```cpp
outcome::result<std::vector<uint8_t>> encode(...)
outcome::result<T> decode(...)
```

Code path: `core/scale/scale_encoder_stream.hpp`  
Namespace: `kagome::scale`
Description: The Class `ScaleEncoderStream` is the primary way of encoding data to SCALE. The class can be overloaded with the `<<` operator and return the result when calling the `.data()` method on the stream.

Function interface(s):
```cpp
class ScaleEncoderStream {
    public:
        // encodes pair of values
        ScaleEncoderStream &operator<<(const std::pair<F, S> &p) { ... }
        // encode variant value
        ScaleEncoderStream &operator<<(const boost::variant<T...> &v) { ... }
        // encode collection of items of the same type
        ScaleEncoderStream &operator<<(const std::vector<T> &c) { ... }
        // encode optional value
        ScaleEncoderStream &operator<<(const boost::optional<T> &v) { ... }
        // appends sequence of bytes
        ScaleEncoderStream &operator<<(const gsl::span<T> &v) { ... }
        // scale-encodes array of items
        ScaleEncoderStream &operator<<(const std::array<T, size> &a) { ... }
        // encodes unsigned 256-bit integer
        ScaleEncoderStream &operator<<(const boost::multiprecision::uint256_t &i) { ... }
        // encodes reference wrapper of a type
        ScaleEncoderStream &operator<<(const std::reference_wrapper<T> &v) { ... }
        // encodes a string view
        ScaleEncoderStream &operator<<(std::string_view sv) { ... }
        // encodes any integral type including bool
        template <typename T,
                typename I = std::decay_t<T>,
                typename = std::enable_if_t<std::is_integral<I>::value>>
        ScaleEncoderStream &operator<<(T &&v) { ... }
        // encodes value as compact integer
        ScaleEncoderStream &operator<<(const CompactInteger &v);
}
```

#### Internal 

### Host API

...
