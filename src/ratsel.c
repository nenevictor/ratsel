#define NEDA_IMPLEMENTATION
#include "ratsel.h"

#include "stb_include.h"

NEDA_BODY_IMPLEMENTATION(RTSLString, 32);

uint64_t RTSLMapFunction_hash(const void *_item, uint64_t _seed0, uint64_t _seed1)
{
    const RTSLMapFunction *function = (RTSLMapFunction *)_item;

    return hashmap_sip(
        function->name,
        strlen(function->name),
        _seed0,
        _seed1);
};

int RTSLMapFunction_compare(const void *_a, const void *_b, void *udata)
{
    RTSL_UNUSED(udata);
    const RTSLMapFunction *sa = _a;
    const RTSLMapFunction *sb = _b;
    return strcmp(sa->name, sb->name);
};

void RTSLApp__appClearArgs(RTSLApp *_app)
{
    for (size_t i = 0; i < neda_RTSLString__size(_app->args); i++)
    {
        RTSLString argument = neda_RTSLString__at(_app->args, i);
        free(argument);
    };

    neda_RTSLString__clear(_app->args);
};

void RTSLApp__consoleStringTokenize(RTSLApp *_app, char *_command_line)
{
    RTSL_UNUSED(_app);
    char *argument = strtok(_command_line, " \t");

    while (argument)
    {
        const size_t argument_size = strlen(argument);
        RTSLString argument_string;
        argument_string = (char *)malloc(sizeof(char) * (argument_size + 1));
        argument_string[argument_size] = 0;

        for (size_t i = 0; i < argument_size; i++)
        {
            argument_string[i] = argument[i];
        };

        neda_RTSLString__push_back(_app->args, argument_string);

        argument = strtok(NULL, " \t");
    };
};

void RTSLApp__consoleGetLine(char *_dest, const size_t _max_length)
{
    char temp_char = 0;
    size_t dest_index = 0;

    while (temp_char != '\n')
    {
        temp_char = getchar();

        if (dest_index < _max_length && temp_char != '\n')
        {
            _dest[dest_index] = temp_char;
            dest_index++;
        };
    };
};

RTSLBuffer RTSLBuffer__readFile(const char *_path)
{
    RTSLBuffer buffer;
    memset(&buffer, 0, sizeof(RTSLBuffer));

    FILE *f = fopen(_path, "rb");
    if (f == 0)
        return buffer;
    fseek(f, 0, SEEK_END);
    buffer.size = (size_t)ftell(f);
    buffer.data = (unsigned char *)malloc(buffer.size);
    if (buffer.data == 0)
        return buffer;

    fseek(f, 0, SEEK_SET);
    fread(buffer.data, sizeof(*buffer.data), buffer.size, f);
    fclose(f);

    return buffer;
};

int RTSLBuffer__writeFile(RTSLBuffer *_buffer, const char *_path)
{
    FILE *file = fopen(_path, "wb");

    if (!file)
    {
        return RTSL_FAILURE;
    };

    fwrite(_buffer->data, sizeof(*_buffer->data), _buffer->size, file);
    fclose(file);

    return RTSL_SUCCESS;
};

void RTSLString__copy(RTSLString _string_src, RTSLString *_string_dest)
{
    const size_t src_len = strlen(_string_src);
    *_string_dest = (char *)malloc(sizeof(char) * (src_len + 1));
    (*_string_dest)[src_len] = 0;

    for (size_t i = 0; i < src_len; i++)
    {
        (*_string_dest)[i] = _string_src[i];
    };
};

void RTSLString__replaceExtension(RTSLString *_path, RTSLString _extension)
{
    size_t path_len = strlen(*_path);
    const size_t extension_len = strlen(_extension);
    size_t dot_index = 0;

    for (size_t i = path_len; i > 0; i--)
    {
        if ((*_path)[i] == '.')
            break;
        dot_index++;
    };
    dot_index = path_len - dot_index;

    if (dot_index == 0)
    {
        dot_index = path_len;
        path_len = path_len + extension_len;
        *_path = realloc(*_path, sizeof(char) * (path_len + 1));
        (*_path)[path_len] = 0;
    }
    else if (extension_len != path_len - dot_index)
    {
        path_len = path_len - (path_len - dot_index) + extension_len;
        *_path = realloc(*_path, sizeof(char) * (path_len + 1));
        (*_path)[path_len] = 0;
    };

    for (size_t i = dot_index, i_e = 0;
         i < path_len && _extension[i_e] != 0;
         i++, i_e++)
    {
        (*_path)[i] = _extension[i_e];
    };
};

void RTSLString__eraseExtension(RTSLString *_path)
{
    size_t path_len = strlen(*_path);
    size_t dot_index = 0;

    for (size_t i = path_len; i > 0; i--)
    {
        if ((*_path)[i] == '.')
            break;
        dot_index++;
    };

    dot_index = path_len - dot_index;

    if (dot_index == 0)
    {
        return;
    };

    *_path = realloc(*_path, sizeof(char) * (dot_index + 1));
    (*_path)[dot_index] = 0;
};

void RTSLString__addExtension(RTSLString *_path, RTSLString _extension)
{
    size_t path_len = strlen(*_path);
    const size_t extension_len = strlen(_extension);

    (*_path)[path_len] = '.';
    path_len += extension_len;

    *_path = realloc(*_path, sizeof(char) * (path_len + 1));
    (*_path)[path_len] = 0;

    for (size_t i = path_len - 1, i_e = extension_len - 1;
         i > 0 && i_e > 0;
         i--, i_e--)
    {
        (*_path)[i] = _extension[i_e];
    };
};

int RTSLString__compareExtension(RTSLString _path, RTSLString _extension)
{
    size_t path_len = strlen(_path);
    const size_t extension_len = strlen(_extension);

    for (size_t i = path_len - 1, i_e = extension_len - 1;
         i > 0 && i_e > 0;
         i--, i_e--)
    {
        if (_path[i] != _extension[i_e])
        {
            return RTSL_FAILURE;
        };
    };
    return RTSL_SUCCESS;
};

int RTSLApp__init(RTSLApp *_app)
{
    _app->functions = hashmap_new(
        sizeof(RTSLMapFunction),
        0,
        0,
        0,
        RTSLMapFunction_hash,
        RTSLMapFunction_compare,
        NULL,
        NULL);

    hashmap_set(_app->functions,
                &(RTSLMapFunction){.name = "help", .function = &RTSLAppFunction__help});

    hashmap_set(_app->functions,
                &(RTSLMapFunction){.name = "encrypt", .function = &RTSLAppFunction__encrypt});

    hashmap_set(_app->functions,
                &(RTSLMapFunction){.name = "decrypt", .function = &RTSLAppFunction__decrypt});

    return RTSL_SUCCESS;
};

int RTSLApp__run(RTSLApp *_app)
{
    clogf("\n%s; Version: %i.%i.%i\n",
          "Ratsel",
          RATSEL_MAJOR,
          RATSEL_MINOR,
          RATSEL_PATCH);

    char *command_line = (char *)malloc(sizeof(char) * RTSL_COMMAND_LINE_MAX);
    neda_RTSLString__init(&_app->args);

    while (1)
    {
        memset((void *)command_line, 0, RTSL_COMMAND_LINE_MAX);

        printf("\n>");
        RTSLApp__consoleGetLine(command_line, RTSL_COMMAND_LINE_MAX);
        RTSLApp__appClearArgs(_app);
        RTSLApp__consoleStringTokenize(_app, command_line);
        const size_t arguments_count = neda_RTSLString__size(_app->args);

        if (arguments_count)
        {
            char *first_argument = neda_RTSLString__at(_app->args, 0);

            if (!strcmp(first_argument, "exit"))
            {
                break;
            };

            RTSLMapFunction *current_function = (RTSLMapFunction *)hashmap_get(
                _app->functions,
                &(RTSLMapFunction){.name = first_argument});

            if (current_function)
            {
                current_function->function(_app);
            }
            else
            {
                clogf(
                    CLOG_WARN "command not found: %s; Use \"help\" to get manual.",
                    first_argument);
            };
        }
        else
        {
            clogf(CLOG_WARN "%s", "too few arguments! Use \"help\" to get manual.");
        };
    };

    free(command_line);
    RTSLApp__appClearArgs(_app);
    neda_RTSLString__free(&_app->args);

    return RTSL_SUCCESS;
};

void RTSLApp__close(RTSLApp *_app)
{
    hashmap_free(_app->functions);

    return;
};

int RTSLAppFunction__encrypt(void *_app)
{
    RTSLApp *app = (RTSLApp *)(_app);

    char *input_path, *key, *output_path = 0;
    size_t key_size;

    if (neda_RTSLString__size(app->args) != 3)
    {
        return RTSL_FAILURE;
    };

    input_path = neda_RTSLString__at(app->args, 1);
    key = neda_RTSLString__at(app->args, 2);
    key_size = strlen(key);

    RTSLString__copy(input_path, &output_path);
    RTSLString__addExtension(&output_path, RTSL_EXTENSION);

    RTSLBuffer file_data = RTSLBuffer__readFile(input_path);

    if (!file_data.data)
    {
        free(output_path);
        clogf(CLOG_WARN "cannot open file: %s", input_path);
        return RTSL_FAILURE;
    };

    for (size_t i = 0; i < file_data.size; i++)
    {
        const size_t key_index = i % key_size;

        file_data.data[i] = file_data.data[i] + key[key_index];
    };

    if (RTSLBuffer__writeFile(&file_data, output_path))
    {
        clogf(CLOG_WARN "cannot save file: %s", output_path);

        free(output_path);
        free(file_data.data);
        return RTSL_SUCCESS;
    };

    clogf(CLOG_INFO "file saved: %s", output_path);
    free(output_path);
    free(file_data.data);

    return RTSL_SUCCESS;
};

int RTSLAppFunction__decrypt(void *_app)
{
    RTSLApp *app = (RTSLApp *)(_app);

    char *input_path, *key, *output_path = 0;
    size_t key_size;

    if (neda_RTSLString__size(app->args) != 3)
    {
        return RTSL_FAILURE;
    };

    input_path = neda_RTSLString__at(app->args, 1);
    key = neda_RTSLString__at(app->args, 2);
    key_size = strlen(key);

    RTSLString__copy(input_path, &output_path);
    if (!RTSLString__compareExtension(input_path, RTSL_EXTENSION))
    {
        RTSLString__eraseExtension(&output_path);
    }
    else
    {
        clogf(CLOG_WARN "file extension is not %s: %s", RTSL_EXTENSION, input_path);
    };

    RTSLBuffer file_data = RTSLBuffer__readFile(input_path);

    if (!file_data.data)
    {
        free(output_path);
        clogf(CLOG_WARN "cannot open file: %s", input_path);
        return RTSL_FAILURE;
    };

    for (size_t i = 0; i < file_data.size; i++)
    {
        const size_t key_index = i % key_size;

        file_data.data[i] = file_data.data[i] - key[key_index];
    };

    if (RTSLBuffer__writeFile(&file_data, output_path))
    {
        clogf(CLOG_WARN "cannot save file: %s", output_path);

        free(output_path);
        free(file_data.data);
        return RTSL_SUCCESS;
    };

    clogf(CLOG_INFO "file saved: %s", output_path);
    free(output_path);
    free(file_data.data);

    return RTSL_SUCCESS;
};

int RTSLAppFunction__help(void *_app)
{
    RTSL_UNUSED(_app);

    clogf("\n%s; Version: %i.%i.%i\n%s",
          "Ratsel",
          RATSEL_MAJOR,
          RATSEL_MINOR,
          RATSEL_PATCH,
          "By nenevictor.\n\n"
          "encrypt <file_path> <password> - encrypt file;\n"
          "decrypt <file_path> <password> - decrypt file;\n"
          "help                           - get some help (sadly, not in real life :();\n"
          "exit                           - exit;\n");

    return RTSL_SUCCESS;
};
