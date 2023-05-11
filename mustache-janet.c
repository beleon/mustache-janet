// based on the pymustach wrapper of mustach (https://github.com/RekGRpth/pymustach), MIT License
#include <janet.h>
#include "mustach.h"
#include "mustach-cjson.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>

static int flags = Mustach_With_AllExtensions;

int mustach_render_cjson_file(const char *template, size_t length, const char *value, size_t buffer_length, int flags, FILE *file, char **err) {
    cJSON *root;
    int rc = MUSTACH_ERROR_USER(1);
    if (!(root = cJSON_ParseWithLength(value, buffer_length))) {
        *err = (char *)cJSON_GetErrorPtr();
        fclose(file);
        return rc;
    }
    rc = mustach_cJSON_file(template, length, root, flags, file);
    cJSON_Delete(root);
    fclose(file);
    return rc;
}

int mustach_render_cjson(const char *template, size_t length, const char *value, size_t buffer_length, int flags, char **result, size_t *size, char **err) {
    cJSON *root;
    int rc = MUSTACH_ERROR_USER(1);
    if (!(root = cJSON_ParseWithLength(value, buffer_length))) {
        *err = (char *)cJSON_GetErrorPtr();
        return rc;
    }
    rc = mustach_cJSON_mem(template, length, root, flags, result, size);
    cJSON_Delete(root);
    return rc;
}


static Janet mustache_render_helper(const char *template, size_t template_len, const char *json, size_t json_len, const char *file_name) {
    char *err;
    char *data = NULL;
    size_t len;
    int mustach_result;
    if (file_name) {
        FILE *file;
        if (!(file = fopen(file_name, "wb"))) {
            janet_panicf("mustache-janet error: failed to open file %s", file_name);
        }
        mustach_result = mustach_render_cjson_file(template, template_len, json, json_len, flags, file, &err);
    } else {
        mustach_result = mustach_render_cjson(template, template_len, json, json_len, flags, &data, &len, &err);
    }
    switch (mustach_result) {
        case MUSTACH_OK: break;
        case MUSTACH_ERROR_SYSTEM: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_SYSTEM");
        case MUSTACH_ERROR_UNEXPECTED_END: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_UNEXPECTED_END");
        case MUSTACH_ERROR_EMPTY_TAG: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_EMPTY_TAG"); 
        case MUSTACH_ERROR_TAG_TOO_LONG: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_TAG_TOO_LONG");
        case MUSTACH_ERROR_BAD_SEPARATORS: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_BAD_SEPARATORS");
        case MUSTACH_ERROR_TOO_DEEP: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_TOO_DEEP");
        case MUSTACH_ERROR_CLOSING: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_CLOSING");
        case MUSTACH_ERROR_BAD_UNESCAPE_TAG: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_BAD_UNESCAPE_TAG");
        case MUSTACH_ERROR_INVALID_ITF: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_INVALID_ITF");
        case MUSTACH_ERROR_ITEM_NOT_FOUND: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_ITEM_NOT_FOUND");
        case MUSTACH_ERROR_PARTIAL_NOT_FOUND: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_PARTIAL_NOT_FOUND");
        case MUSTACH_ERROR_UNDEFINED_TAG: if (data) free(data); janet_panic("mustache-janet error: MUSTACH_ERROR_UNDEFINED_TAG");
        case MUSTACH_ERROR_USER(1): if (data) free(data); janet_panicf("mustache-janet error: cJSON failed to parse at: %s", err);
        default: if (data) free(data); janet_panicf("mustache-janet error: unknown mustach error: %d", mustach_result);
    }

    if (file_name) {
        return janet_wrap_nil();
    } else {
        Janet result = janet_stringv((const uint8_t *) data, len);
        free(data);
        return result;
    }
}

static Janet mustache_render_file(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 3);
    JanetString template = janet_getstring(argv, 0);
    JanetString json = janet_getstring(argv, 1);
    JanetString file_name = janet_getstring(argv, 2);

    size_t template_len = janet_string_length(template);
    size_t json_len = janet_string_length(json);

    return mustache_render_helper((const char *) template, template_len, (const char *) json, json_len, (const char *) file_name);
}

static Janet mustache_render(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);
    JanetString template = janet_getstring(argv, 0);
    JanetString json = janet_getstring(argv, 1);

    size_t template_len = janet_string_length(template);
    size_t json_len = janet_string_length(json);

    return mustache_render_helper((const char *) template, template_len, (const char *) json, json_len, NULL);
}

/****************/
/* Module Entry */
/****************/

static const JanetReg cfuns[] = {
    {"render", mustache_render, "(mustache/render template json)\n\nRenders the mustache template for the provided json."},
    {"render-file", mustache_render_file, "(mustache/render-file template json path)\n\nRenders the mustache template to the file for the provided json."},
    {NULL, NULL, NULL}
};

JANET_MODULE_ENTRY(JanetTable *env) {
    janet_cfuns(env, "mustache", cfuns);
}
