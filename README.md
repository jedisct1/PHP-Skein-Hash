# Skein hash functions for PHP

PHP extension implementing the [Skein](https://www.schneier.com/academic/skein/) cryptographic hash function family (256, 512, 1024-bit internal state).

Skein is a family of cryptographic hash functions designed by Niels Ferguson, Stefan Lucks, Bruce Schneier, Doug Whiting, Mihir Bellare, Tadayoshi Kohno, Jon Callas, and Jesse Walker. It was a finalist in the NIST SHA-3 competition.

This extension wraps Doug Whiting's optimized reference C implementation with precomputed initialization vectors.

## Installation

Requirements: PHP 7.0 or later with development headers installed.

On Debian/Ubuntu: `apt install php-dev`

Build and install like any standard PHP extension:

```sh
phpize
./configure --enable-skein
make
make install
```

Then add to your `php.ini`:

```ini
extension=skein.so
```

## Usage

Get the binary 512-bit hash of a string:

```php
$hash = skein_hash($data);
```

Get the hexadecimal 512-bit hash of a string:

```php
$hex_hash = skein_hash_hex($data);
```

Specify a custom bit length (1-1024):

```php
$hash = skein_hash($data, 256);
$hex_hash = skein_hash_hex($data, 1024);
```

The appropriate Skein variant is selected automatically based on the requested bit length:

- 1-256 bits: Skein-256
- 257-512 bits: Skein-512
- 513-1024 bits: Skein-1024

The default bit length is 512.

## Functions

### `skein_hash(string $data, int $bitlen = 512): string|false`

Returns the raw binary hash, or `false` on error.

### `skein_hash_hex(string $data, int $bitlen = 512): string|false`

Returns the lowercase hexadecimal hash, or `false` on error.

## Distribution

Skein hash function by Doug Whiting.
Skein hash function family home page: https://www.skein-hash.info/

PHP extension by Frank Denis.

GitHub: https://github.com/jedisct1/PHP-Skein-Hash

License: Public domain.
