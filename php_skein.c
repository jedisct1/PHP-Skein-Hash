#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "skein.h"
#include "php_skein.h"

#define SKEIN_DEFAULT_BITS_LENGTH 512

ZEND_BEGIN_ARG_INFO_EX(arginfo_skein_hash, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, bitlen)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_skein_hash_hex, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, bitlen)
ZEND_END_ARG_INFO()

static const zend_function_entry skein_functions[] = {
	PHP_FE(skein_hash, arginfo_skein_hash)
	PHP_FE(skein_hash_hex, arginfo_skein_hash_hex)
	PHP_FE_END
};

zend_module_entry skein_module_entry = {
	STANDARD_MODULE_HEADER,
	"skein",
	skein_functions,
	PHP_MINIT(skein),
	PHP_MSHUTDOWN(skein),
	NULL,
	NULL,
	PHP_MINFO(skein),
	"2.0",
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SKEIN
ZEND_GET_MODULE(skein)
#endif

PHP_MINIT_FUNCTION(skein)
{
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(skein)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(skein)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "skein hash support", "enabled");
	php_info_print_table_end();
}

static int skein256_hash_buffer(unsigned char hash[32],
                                const size_t sizeof_hash,
                                const unsigned char *buf,
                                const size_t buf_size,
                                const size_t hash_bitlen)
{
    Skein_256_Ctxt_t ctx;
    
    if (sizeof_hash < 32U) {
        return -1;
    }
    memset(hash, 0, 32U);
    Skein_256_Init(&ctx, hash_bitlen);
    Skein_256_Update(&ctx, (const u08b_t *) buf, buf_size);
    Skein_256_Final(&ctx, hash);
    
    return 0;
}

static int skein512_hash_buffer(unsigned char hash[64],
                                const size_t sizeof_hash,
                                const unsigned char *buf,
                                const size_t buf_size,
                                const size_t hash_bitlen)
{
    Skein_512_Ctxt_t ctx;
    
    if (sizeof_hash < 64U) {
        return -1;
    }
    memset(hash, 0, 64U);
    Skein_512_Init(&ctx, hash_bitlen);
    Skein_512_Update(&ctx, (const u08b_t *) buf, buf_size);
    Skein_512_Final(&ctx, hash);
    
    return 0;
}

static int skein1024_hash_buffer(unsigned char hash[128],
                                 const size_t sizeof_hash,
                                 const unsigned char *buf,
                                 const size_t buf_size,
                                 const size_t hash_bitlen)
{
    Skein1024_Ctxt_t ctx;
    
    if (sizeof_hash < 128U) {
        return -1;
    }
    memset(hash, 0, 128U);
    Skein1024_Init(&ctx, hash_bitlen);
    Skein1024_Update(&ctx, (const u08b_t *) buf, buf_size);
    Skein1024_Final(&ctx, hash);

    return 0;
}

static int skein_hash_buffer(unsigned char * const hash,
                             const size_t sizeof_hash,
                             const unsigned char *buf,
                             const size_t buf_size,
                             const size_t hash_bitlen)
{
    if (hash_bitlen <= 256) {
        return skein256_hash_buffer(hash, sizeof_hash, buf, buf_size,
                                    hash_bitlen);
    } else if (hash_bitlen <= 512) {
        return skein512_hash_buffer(hash, sizeof_hash, buf, buf_size,
                                    hash_bitlen);
    }
    return skein1024_hash_buffer(hash, sizeof_hash,
                                 buf, buf_size, hash_bitlen);
}

PHP_FUNCTION(skein_hash)
{
    char hash[128];
	char *buf = NULL;
	size_t buf_size;
	zend_long hash_bitlen = SKEIN_DEFAULT_BITS_LENGTH;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l",
                              &buf, &buf_size, &hash_bitlen) == FAILURE) {
		RETURN_THROWS();
	}
    if (hash_bitlen < 1 || hash_bitlen > 1024) {
        php_error_docref(NULL, E_WARNING, "Bad bit length");
        RETURN_FALSE;
    }
    if (skein_hash_buffer((unsigned char *) hash, sizeof hash,
                          (const unsigned char *) buf, buf_size,
                          (size_t) hash_bitlen) != 0) {
        RETURN_FALSE;
    }
    RETURN_STRINGL(hash, (size_t) ((hash_bitlen + 7) / 8));
}

PHP_FUNCTION(skein_hash_hex)
{
	static const char hexits[] = "0123456789abcdef";
    unsigned char hash[128];
	char res[256];
	char *resptr = res;
	char *buf = NULL;
	size_t buf_size;
    size_t j = 0U;
    size_t real_hash_size;
	zend_long hash_bitlen = SKEIN_DEFAULT_BITS_LENGTH;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l",
                              &buf, &buf_size, &hash_bitlen) == FAILURE) {
		RETURN_THROWS();
	}
    if (hash_bitlen < 1 || hash_bitlen > 1024) {
        php_error_docref(NULL, E_WARNING, "Bad bit length");
        RETURN_FALSE;
    }
    if (skein_hash_buffer(hash, sizeof hash,
                          (const unsigned char *) buf, buf_size,
                          (size_t) hash_bitlen) != 0) {
        RETURN_FALSE;
    }
    real_hash_size = (size_t) ((hash_bitlen + 7) / 8);
    if (real_hash_size > sizeof hash) {
        abort();
    }
    do {
        *resptr++ = hexits[(hash[j] & 0xf0) >> 4];
        *resptr++ = hexits[hash[j] & 0x0f];
    } while (++j < real_hash_size);
    *resptr = 0;

	RETURN_STRINGL(res, (size_t) (resptr - res));
}
