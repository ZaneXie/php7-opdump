/*
 +----------------------------------------------------------------------+
 | PHP Version 7                                                        |
 +----------------------------------------------------------------------+
 | Copyright (c) 1997-2015 The PHP Group                                |
 +----------------------------------------------------------------------+
 | This source file is subject to version 3.01 of the PHP license,      |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.php.net/license/3_01.txt                                  |
 | If you did not receive a copy of the PHP license and are unable to   |
 | obtain it through the world-wide-web, please send a note to          |
 | license@php.net so we can mail you a copy immediately.               |
 +----------------------------------------------------------------------+
 | Author:                                                              |
 +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_opdump.h"
#include "zend_vm_opcodes.h"
#include "zend_compile.h"

/* If you declare any globals in php_opdump.h uncomment this:
 ZEND_DECLARE_MODULE_GLOBALS(opdump)
 */

/* True global resources - no need for thread safety here */
static int le_opdump;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
 PHP_INI_BEGIN()
 STD_PHP_INI_ENTRY("opdump.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_opdump_globals, opdump_globals)
 STD_PHP_INI_ENTRY("opdump.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_opdump_globals, opdump_globals)
 PHP_INI_END()
 */
/* }}} */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_opdump_compiled(string arg)
 Return a string to confirm that the module is compiled in */
PHP_FUNCTION(opdump_test) {
	char *arg = NULL;
	size_t arg_len, len;
	zend_string *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len)
			== FAILURE) {
		return;
	}

	strg = strpprintf(0, "hehe");

	RETURN_STR(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
 unfold functions in source code. See the corresponding marks just before
 function definition, where the functions purpose is also documented. Please
 follow this convention for the convenience of others editing your code.
 */

/* {{{ php_opdump_init_globals
 */
/* Uncomment this function if you have INI entries
 static void php_opdump_init_globals(zend_opdump_globals *opdump_globals)
 {
 opdump_globals->global_value = 0;
 opdump_globals->global_string = NULL;
 }
 */
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(opdump) {
	/* If you have INI entries, uncomment these lines
	 REGISTER_INI_ENTRIES();
	 */
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(opdump) {
	/* uncomment this line if you have INI entries
	 UNREGISTER_INI_ENTRIES();
	 */
	return SUCCESS;
}
/* }}} */

ZEND_API zend_op_array *(*origin_zend_compile_file)(
		zend_file_handle *file_handle, int type);
ZEND_API zend_op_array *(*origin_zend_compile_string)(zval *source_string,
		char *filename);

FILE* op_file;
size_t opdump_printf(const char *format, ...) {
	char * message;
	va_list args;
	va_start(args, format);
	int ret = vspprintf(&message, 0, format, args);
	va_end(args);
	fprintf(op_file, "%s", message);
	efree(message);
	return ret;
}

void format_zval_string(char* buffer, zval*z){
	zend_string* str = Z_STR_P(z);
	php_sprintf(buffer, "string:%s", str->val);
}

void format_zval_long(char* buffer, zval*z){
	zend_long l= Z_LVAL_P(z);
	php_sprintf(buffer, "long:%ld", l);
}
void format_zval_double(char* buffer, zval*z){
	double d= Z_DVAL_P(z);
	php_sprintf(buffer, "double:%.10lf", d);
}
void format_zval(char* buffer, zval* z){
	switch(z->u1.v.type){
	case IS_UNDEF:
		strcpy(buffer, "undef");
		break;
	case IS_NULL:
		strcpy(buffer, "null");
		break;
	case IS_FALSE:
		strcpy(buffer, "false");
		break;
	case IS_TRUE:
		strcpy(buffer, "true");
		break;
	case IS_LONG:
		format_zval_long(buffer, z);
		break;
	case IS_DOUBLE:
		format_zval_double(buffer, z);
		break;
	case IS_STRING:
		format_zval_string(buffer, z);
		break;
	case IS_ARRAY:
		strcpy(buffer, "array");
		break;
	case IS_OBJECT:
		strcpy(buffer, "object");
		break;
	case IS_RESOURCE:
		strcpy(buffer, "resource");
		break;
	case IS_REFERENCE:
		strcpy(buffer, "reference");
		break;
	default:
		strcpy(buffer, "unkown_zval");
		break;
	}
}

void format_znode_op(char* buffer, zend_uchar type, znode_op op,
		zend_op_array* op_array) {
	zval* z;
	switch (type) {
	case IS_CONST:
		z = RT_CONSTANT_EX(op_array->literals, op);
		format_zval(buffer, z);
		break;
	case IS_TMP_VAR:
		php_sprintf(buffer, "TMPVAR+%d", op.jmp_offset);
		break;
	case IS_VAR:
		php_sprintf(buffer, "VAR+%d", op.jmp_offset);
		break;
	case IS_UNUSED:
		strcpy(buffer, "unused");
		break;
	case IS_CV:
		php_sprintf(buffer, "CV+%d", op.jmp_offset);
		break;
	default:
		strcpy(buffer, "unknown");
		break;
	}
}

void print_opcode(zend_op* op_code, zend_op_array* op_array) {
	static char buffer_op1[200];
	static char buffer_op2[200];
	static char buffer_result[200];
	format_znode_op(buffer_op1, op_code->op1_type, op_code->op1, op_array);
	format_znode_op(buffer_op2, op_code->op2_type, op_code->op2, op_array);
	format_znode_op(buffer_result, op_code->result_type, op_code->result, op_array);

	opdump_printf("%20s\t%s\t%s\t%s\n", zend_get_opcode_name(op_code->opcode),
			buffer_op1, buffer_op2, buffer_result);
}

ZEND_API zend_op_array *opdump_compile_file(zend_file_handle *file_handle,
		int type) {
	int i;

	char op_filename[500];
	zend_op_array* ret = origin_zend_compile_file(file_handle, type);
	strncpy(op_filename, file_handle->filename, sizeof(op_filename));
	strcat(op_filename, ".opcodes");

	op_file = fopen(op_filename, "w");

	for (i = 0; i < ret->last; i++) {
		print_opcode(ret->opcodes + i, ret);
	}

	fclose(op_file);
	return ret;
}

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(opdump) {
#if defined(COMPILE_DL_OPDUMP) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	origin_zend_compile_file = zend_compile_file;
	zend_compile_file = opdump_compile_file;
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(opdump) {
	zend_compile_file = origin_zend_compile_file;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(opdump) {
	php_info_print_table_start();
	php_info_print_table_header(2, "opdump support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	 DISPLAY_INI_ENTRIES();
	 */
}
/* }}} */

/* {{{ opdump_functions[]
 *
 * Every user visible function must have an entry in opdump_functions[].
 */
const zend_function_entry opdump_functions[] = {
PHP_FE(opdump_test, NULL) /* For testing, remove later. */
PHP_FE_END /* Must be the last line in opdump_functions[] */
};
/* }}} */

/* {{{ opdump_module_entry
 */
zend_module_entry opdump_module_entry = {
STANDARD_MODULE_HEADER, "opdump", opdump_functions,
PHP_MINIT(opdump),
PHP_MSHUTDOWN(opdump),
PHP_RINIT(opdump), /* Replace with NULL if there's nothing to do at request start */
PHP_RSHUTDOWN(opdump), /* Replace with NULL if there's nothing to do at request end */
PHP_MINFO(opdump),
PHP_OPDUMP_VERSION,
STANDARD_MODULE_PROPERTIES };
/* }}} */

#ifdef COMPILE_DL_OPDUMP
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(opdump)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
