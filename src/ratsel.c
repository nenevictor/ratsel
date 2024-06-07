#include "ratsel.h"

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

void RTSLApp__getLine(char *_dest, const size_t _max_length)
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

    // Remove spaces from end.
    for (size_t i = dest_index - 1; i > 0; i--)
    {
        if (_dest[i] == ' ' || _dest[i] == '\t')
        {
            _dest[i] = 0;
        }
        else
        {
            break;
        };
    };
};

int RTSLApp__init(RTSLApp *_app)
{
    _app->command_line = (char *)malloc(sizeof(char) * RTSL_COMMAND_LINE_SIZE);

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
                &(RTSLMapFunction){.name = "--help", .function = &RTSLAppFunction__help});

    hashmap_set(_app->functions,
                &(RTSLMapFunction){.name = "encrypt", .function = &RTSLAppFunction__encrypt});

    hashmap_set(_app->functions,
                &(RTSLMapFunction){.name = "decrypt", .function = &RTSLAppFunction__decrypt});

    return RTSL_SUCCESS;
};

int RTSLApp__run(RTSLApp *_app)
{
    RTSLMapFunction *current_function;

    clogf("\n%s; Version: %i.%i.%i\n",
          "Ratsel",
          RATSEL_MAJOR,
          RATSEL_MINOR,
          RATSEL_PATCH);

    while (1)
    {
        memset((void *)_app->command_line, 0, RTSL_COMMAND_LINE_SIZE);
        memset((void *)_app->arg0, 0, RTSL_COMMAND_LINE_SIZE);
        memset((void *)_app->arg1, 0, RTSL_COMMAND_LINE_SIZE);
        memset((void *)_app->arg2, 0, RTSL_COMMAND_LINE_SIZE);

        printf("\n>");
        RTSLApp__getLine(_app->command_line, RTSL_COMMAND_LINE_SIZE);

        {
            char *command = strtok(_app->command_line, " \t");
            if (!command)
            {
                clogf(
                    CLOG_WARN "Command not found: %s;\n" CLOG_INFO "Use --help to get manual.",
                    command);

                continue;
            };

            d_clogf("%s", command);
            strcpy(_app->arg0, command);

            command = strtok(NULL, " ");
            if (command)
            {
                d_clogf("%s", command);
                strcpy(_app->arg1, command);
            };

            command = strtok(NULL, " ");
            if (command)
            {
                d_clogf("%s", command);
                strcpy(_app->arg2, command);
            };
        };

        if (!strcmp(_app->arg0, "exit"))
        {
            break;
        };

        current_function = (RTSLMapFunction *)hashmap_get(
            _app->functions,
            &(RTSLMapFunction){.name = _app->arg0});

        if (current_function)
        {
            current_function->function(_app);
        }
        else
        {
            clogf(
                CLOG_WARN "Command not found: %s;\n" CLOG_INFO "Use --help to get manual.",
                _app->arg0);
        };
    };

    return RTSL_SUCCESS;
};

int RTSLApp__close(RTSLApp *_app)
{
    free(_app->command_line);
    hashmap_free(_app->functions);

    return RTSL_SUCCESS;
};

int RTSLAppFunction__encrypt(void *_app)
{
    RTSLApp *app = (RTSLApp *)(_app);

    char output_file_path[RTSL_COMMAND_LINE_SIZE];

    char *buffer;
    size_t file_size;

    if (!app->arg1[0] || !app->arg2[0])
    {
        clogf("%s", CLOG_WARN "Too few arguments!\n" CLOG_INFO "Use --help to get manual.");
        return EXIT_FAILURE;
    };

    // Set output filename.
    {
        const size_t output_file_path_size = strlen(app->arg1);
        const size_t output_file_extension_size = strlen(RTSL_EXTENSION);

        strcpy(output_file_path, app->arg1);

        for (size_t i = 0; i < output_file_extension_size; i++)
        {
            output_file_path[i + output_file_path_size] = RTSL_EXTENSION[i];
        };

        output_file_path[output_file_extension_size + output_file_path_size] = 0;
    };

    // Read input file.
    {
        FILE *file;

        file = fopen(app->arg1, "rb");

        if (!file)
        {
            clogf("%s%s", CLOG_WARN "Could not open file: ", app->arg1);
            return EXIT_FAILURE;
        };

        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);

        buffer = (char *)malloc(sizeof(char) * file_size);

        if (!buffer)
        {
            return EXIT_FAILURE;
        };

        fread(buffer, file_size, 1, file);
        fclose(file);
    };

    // Simple encrypt.
    if (buffer)
    {
        size_t key_size = strlen(app->arg2);
        for (size_t i = 0; i < file_size; i++)
        {
            const size_t key_index = i % key_size;

            buffer[i] = buffer[i] + app->arg2[key_index];
        };
    };

    // Write output file.
    if (buffer)
    {
        FILE *file;

        file = fopen(output_file_path, "wb");

        if (!file)
        {
            clogf("%s%s", CLOG_WARN "Could not save file: ", output_file_path);
            return EXIT_FAILURE;
        };

        fwrite(buffer, file_size, 1, file);
        fclose(file);
    };
    
    free(buffer);

    return RTSL_SUCCESS;
};

int RTSLAppFunction__decrypt(void *_app)
{
    RTSLApp *app = (RTSLApp *)(_app);

    char output_file_path[RTSL_COMMAND_LINE_SIZE];

    char *buffer = 0;
    size_t file_size;

    if (!app->arg1[0] || !app->arg2[0])
    {
        clogf("%s", CLOG_WARN "Too few arguments!\n" CLOG_INFO "Use --help to get manual.");
        return EXIT_FAILURE;
    };

    // Set output filename.
    {
        const size_t output_file_path_size = strlen(app->arg1);
        const size_t output_file_extension_size = strlen(RTSL_EXTENSION);

        if (output_file_path_size < output_file_extension_size + 1)
        {
            return EXIT_FAILURE;
        };

        strcpy(output_file_path, app->arg1);

        const char *output_file_path_ext =
            output_file_path + (output_file_path_size - output_file_extension_size);

        if (strcmp(output_file_path_ext, RTSL_EXTENSION))
        {
            clogf("%s%s", CLOG_WARN "file extension is not .rtsl: ", output_file_path);
            return EXIT_FAILURE;
        };

        output_file_path[output_file_path_size - output_file_extension_size] = 0;
    };

    // Read input file.
    {
        FILE *file;

        file = fopen(app->arg1, "rb");

        if (!file)
        {
            clogf("%s%s", CLOG_WARN "Could not open file: ", app->arg1);
            return EXIT_FAILURE;
        };

        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);

        buffer = (char *)malloc(sizeof(char) * file_size);

        if (!buffer)
        {
            return EXIT_FAILURE;
        };

        fread(buffer, file_size, 1, file);
        fclose(file);
    };

    // Simple decrypt.
    if (buffer)
    {
        size_t key_size = strlen(app->arg2);
        for (size_t i = 0; i < file_size; i++)
        {
            const size_t key_index = i % key_size;

            buffer[i] = buffer[i] - app->arg2[key_index];
        };
    };

    // Write output file.
    if (buffer)
    {
        FILE *file;

        file = fopen(output_file_path, "wb");

        if (!file)
        {
            clogf("%s%s", CLOG_WARN "Could not save file: ", output_file_path);
            return EXIT_FAILURE;
        };

        fwrite(buffer, file_size, 1, file);
        fclose(file);
    };

    free(buffer);

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
          "--help                         - get some help (sadly, not in real life :();\n"
          "exit                           - exit;\n");

    return RTSL_SUCCESS;
};
