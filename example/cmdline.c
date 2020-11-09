/*
  File autogenerated by gengetopt version 2.22.6
  generated with the following command:
  gengetopt -i cmdline.ggo --func-name=cmdline_parser

  The developers of gengetopt consider the fixed text that goes in all
  gengetopt output files to be in the public domain:
  we make no copyright claims on it.
*/

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FIX_UNUSED
#define FIX_UNUSED(X) (void) (X) /* avoid warnings for unused params */
#endif

#include <getopt.h>

#include "cmdline.h"

const char *gengetopt_args_info_purpose = "";

const char *gengetopt_args_info_usage = "Usage: TCIP4 Emulator [OPTIONS]...";

const char *gengetopt_args_info_versiontext = "";

const char *gengetopt_args_info_description = "";

const char *gengetopt_args_info_help[] = {
  "  -h, --help                    Print help and exit",
  "  -V, --version                 Print version and exit",
  "  -f, --firmware-version=88 or 96\n                                TCIP4 firmware version to emulate.  (possible\n                                  values=\"88\", \"96\")",
  "  -p, --pipe-name=filename      Filename of where a link to the virtual serial\n                                  port should be made.",
  "  -s, --sweep=PARAMETER         Set parameter to sweep  (possible\n                                  values=\"RPM\", \"SPEED\", \"SENSOR_VALUE\",\n                                  \"SENSORMV\", \"SENSOR_TYPE\", \"BATTMV\",\n                                  \"PROGRAMMINGS\", \"SERVO_MEASURED\",\n                                  \"SERVO_REQUESTED\", \"NUM_CYLINDERS\",\n                                  \"CH1_MAXADVANCE\", \"CH2_MAXADVANCE\",\n                                  \"CH3_MAXADVANCE\", \"CH4_MAXADVANCE\",\n                                  \"DWELL_OPT\", \"DWELL\", \"RUNTIME\",\n                                  \"PROP1\", \"PROP2\", \"PROP3\", \"PROP4\",\n                                  \"RESPONSE_NUMBER\", \"CH1_ADVANCE\",\n                                  \"CH2_ADVANCE\", \"CH3_ADVANCE\",\n                                  \"CH4_ADVANCE\", \"LIMITER\",\n                                  \"START_LIMITER\", \"RETARD\",\n                                  \"CLUTCH_MASTER\", \"POWER_OUT\",\n                                  \"FLAGS_V88_90\", \"FLAGS_V88_91\",\n                                  \"FLAGS_V96_140\", \"FLAGS_V96_141\",\n                                  \"FLAGS_V96_142\", \"FLAGS_V96_143\",\n                                  \"FLAGS_V96_144\", \"FLAGS_V96_145\"\n                                  default=`RPM')",
  "  -d, --data-file=filename      Data file in binary form. File must contain\n                                  properly sized packets. If this is specified,\n                                  '--sweep' is ignored.",
  "  -i, --interactive             User input required to change output packet.",
  "  -x, --hex-output              Output a HEX representation of each packet\n                                  sent.",
  "  -v, --v                       Verbose output. Specify multiple times for\n                                  increasing verbosity.",
  "      --verbose=VERBOSITY       Set level of verbosity explicitly. \n                                  (Overrides -v)\n                                    (possible values=\"NONE\", \"ERROR\",\n                                  \"WARN\", \"INFO\", \"DEBUG\"\n                                  default=`DEBUG')",
  "  -q, --quiet                   Suppress output. Same as --verbose=NONE\n                                  (Overrides both -v and --verbose)",
    0
};

typedef enum {ARG_NO
  , ARG_STRING
  , ARG_INT
  , ARG_ENUM
} cmdline_parser_arg_type;

static
void clear_given (struct gengetopt_args_info *args_info);
static
void clear_args (struct gengetopt_args_info *args_info);

static int
cmdline_parser_internal (int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error);

static int
cmdline_parser_required2 (struct gengetopt_args_info *args_info, const char *prog_name, const char *additional_error);

const char *cmdline_parser_firmware_version_values[] = {"88", "96", 0}; /*< Possible values for firmware-version. */
const char *cmdline_parser_sweep_values[] = {"RPM", "SPEED", "SENSOR_VALUE", "SENSORMV", "SENSOR_TYPE", "BATTMV", "PROGRAMMINGS", "SERVO_MEASURED", "SERVO_REQUESTED", "NUM_CYLINDERS", "CH1_MAXADVANCE", "CH2_MAXADVANCE", "CH3_MAXADVANCE", "CH4_MAXADVANCE", "DWELL_OPT", "DWELL", "RUNTIME", "PROP1", "PROP2", "PROP3", "PROP4", "RESPONSE_NUMBER", "CH1_ADVANCE", "CH2_ADVANCE", "CH3_ADVANCE", "CH4_ADVANCE", "LIMITER", "START_LIMITER", "RETARD", "CLUTCH_MASTER", "POWER_OUT", "FLAGS_V88_90", "FLAGS_V88_91", "FLAGS_V96_140", "FLAGS_V96_141", "FLAGS_V96_142", "FLAGS_V96_143", "FLAGS_V96_144", "FLAGS_V96_145", 0}; /*< Possible values for sweep. */
const char *cmdline_parser_verbose_values[] = {"NONE", "ERROR", "WARN", "INFO", "DEBUG", 0}; /*< Possible values for verbose. */

static char *
gengetopt_strdup (const char *s);

static
void clear_given (struct gengetopt_args_info *args_info)
{
  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->firmware_version_given = 0 ;
  args_info->pipe_name_given = 0 ;
  args_info->sweep_given = 0 ;
  args_info->data_file_given = 0 ;
  args_info->interactive_given = 0 ;
  args_info->hex_output_given = 0 ;
  args_info->v_given = 0 ;
  args_info->verbose_given = 0 ;
  args_info->quiet_given = 0 ;
}

static
void clear_args (struct gengetopt_args_info *args_info)
{
  FIX_UNUSED (args_info);
  args_info->firmware_version_orig = NULL;
  args_info->pipe_name_arg = NULL;
  args_info->pipe_name_orig = NULL;
  args_info->sweep_arg = sweep_arg_RPM;
  args_info->sweep_orig = NULL;
  args_info->data_file_arg = NULL;
  args_info->data_file_orig = NULL;
  args_info->verbose_arg = verbose_arg_DEBUG;
  args_info->verbose_orig = NULL;
  
}

static
void init_args_info(struct gengetopt_args_info *args_info)
{


  args_info->help_help = gengetopt_args_info_help[0] ;
  args_info->version_help = gengetopt_args_info_help[1] ;
  args_info->firmware_version_help = gengetopt_args_info_help[2] ;
  args_info->pipe_name_help = gengetopt_args_info_help[3] ;
  args_info->sweep_help = gengetopt_args_info_help[4] ;
  args_info->data_file_help = gengetopt_args_info_help[5] ;
  args_info->interactive_help = gengetopt_args_info_help[6] ;
  args_info->hex_output_help = gengetopt_args_info_help[7] ;
  args_info->v_help = gengetopt_args_info_help[8] ;
  args_info->v_min = 1;
  args_info->v_max = 3;
  args_info->verbose_help = gengetopt_args_info_help[9] ;
  args_info->quiet_help = gengetopt_args_info_help[10] ;
  
}

void
cmdline_parser_print_version (void)
{
  printf ("%s %s\n",
     (strlen(CMDLINE_PARSER_PACKAGE_NAME) ? CMDLINE_PARSER_PACKAGE_NAME : CMDLINE_PARSER_PACKAGE),
     CMDLINE_PARSER_VERSION);

  if (strlen(gengetopt_args_info_versiontext) > 0)
    printf("\n%s\n", gengetopt_args_info_versiontext);
}

static void print_help_common(void) {
  cmdline_parser_print_version ();

  if (strlen(gengetopt_args_info_purpose) > 0)
    printf("\n%s\n", gengetopt_args_info_purpose);

  if (strlen(gengetopt_args_info_usage) > 0)
    printf("\n%s\n", gengetopt_args_info_usage);

  printf("\n");

  if (strlen(gengetopt_args_info_description) > 0)
    printf("%s\n\n", gengetopt_args_info_description);
}

void
cmdline_parser_print_help (void)
{
  int i = 0;
  print_help_common();
  while (gengetopt_args_info_help[i])
    printf("%s\n", gengetopt_args_info_help[i++]);
}

void
cmdline_parser_init (struct gengetopt_args_info *args_info)
{
  clear_given (args_info);
  clear_args (args_info);
  init_args_info (args_info);
}

void
cmdline_parser_params_init(struct cmdline_parser_params *params)
{
  if (params)
    { 
      params->override = 0;
      params->initialize = 1;
      params->check_required = 1;
      params->check_ambiguity = 0;
      params->print_errors = 1;
    }
}

struct cmdline_parser_params *
cmdline_parser_params_create(void)
{
  struct cmdline_parser_params *params = 
    (struct cmdline_parser_params *)malloc(sizeof(struct cmdline_parser_params));
  cmdline_parser_params_init(params);  
  return params;
}

static void
free_string_field (char **s)
{
  if (*s)
    {
      free (*s);
      *s = 0;
    }
}


static void
cmdline_parser_release (struct gengetopt_args_info *args_info)
{

  free_string_field (&(args_info->firmware_version_orig));
  free_string_field (&(args_info->pipe_name_arg));
  free_string_field (&(args_info->pipe_name_orig));
  free_string_field (&(args_info->sweep_orig));
  free_string_field (&(args_info->data_file_arg));
  free_string_field (&(args_info->data_file_orig));
  free_string_field (&(args_info->verbose_orig));
  
  

  clear_given (args_info);
}

/**
 * @param val the value to check
 * @param values the possible values
 * @return the index of the matched value:
 * -1 if no value matched,
 * -2 if more than one value has matched
 */
static int
check_possible_values(const char *val, const char *values[])
{
  int i, found, last;
  size_t len;

  if (!val)   /* otherwise strlen() crashes below */
    return -1; /* -1 means no argument for the option */

  found = last = 0;

  for (i = 0, len = strlen(val); values[i]; ++i)
    {
      if (strncmp(val, values[i], len) == 0)
        {
          ++found;
          last = i;
          if (strlen(values[i]) == len)
            return i; /* exact macth no need to check more */
        }
    }

  if (found == 1) /* one match: OK */
    return last;

  return (found ? -2 : -1); /* return many values or none matched */
}


static void
write_into_file(FILE *outfile, const char *opt, const char *arg, const char *values[])
{
  int found = -1;
  if (arg) {
    if (values) {
      found = check_possible_values(arg, values);      
    }
    if (found >= 0)
      fprintf(outfile, "%s=\"%s\" # %s\n", opt, arg, values[found]);
    else
      fprintf(outfile, "%s=\"%s\"\n", opt, arg);
  } else {
    fprintf(outfile, "%s\n", opt);
  }
}

static void
write_multiple_into_file(FILE *outfile, int len, const char *opt, char **arg, const char *values[])
{
  int i;
  
  for (i = 0; i < len; ++i)
    write_into_file(outfile, opt, (arg ? arg[i] : 0), values);
}

int
cmdline_parser_dump(FILE *outfile, struct gengetopt_args_info *args_info)
{
  int i = 0;

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot dump options to stream\n", CMDLINE_PARSER_PACKAGE);
      return EXIT_FAILURE;
    }

  if (args_info->help_given)
    write_into_file(outfile, "help", 0, 0 );
  if (args_info->version_given)
    write_into_file(outfile, "version", 0, 0 );
  if (args_info->firmware_version_given)
    write_into_file(outfile, "firmware-version", args_info->firmware_version_orig, cmdline_parser_firmware_version_values);
  if (args_info->pipe_name_given)
    write_into_file(outfile, "pipe-name", args_info->pipe_name_orig, 0);
  if (args_info->sweep_given)
    write_into_file(outfile, "sweep", args_info->sweep_orig, cmdline_parser_sweep_values);
  if (args_info->data_file_given)
    write_into_file(outfile, "data-file", args_info->data_file_orig, 0);
  if (args_info->interactive_given)
    write_into_file(outfile, "interactive", 0, 0 );
  if (args_info->hex_output_given)
    write_into_file(outfile, "hex-output", 0, 0 );
  write_multiple_into_file(outfile, args_info->v_given, "v", 0, 0);
  if (args_info->verbose_given)
    write_into_file(outfile, "verbose", args_info->verbose_orig, cmdline_parser_verbose_values);
  if (args_info->quiet_given)
    write_into_file(outfile, "quiet", 0, 0 );
  

  i = EXIT_SUCCESS;
  return i;
}

int
cmdline_parser_file_save(const char *filename, struct gengetopt_args_info *args_info)
{
  FILE *outfile;
  int i = 0;

  outfile = fopen(filename, "w");

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot open file for writing: %s\n", CMDLINE_PARSER_PACKAGE, filename);
      return EXIT_FAILURE;
    }

  i = cmdline_parser_dump(outfile, args_info);
  fclose (outfile);

  return i;
}

void
cmdline_parser_free (struct gengetopt_args_info *args_info)
{
  cmdline_parser_release (args_info);
}

/** @brief replacement of strdup, which is not standard */
char *
gengetopt_strdup (const char *s)
{
  char *result = 0;
  if (!s)
    return result;

  result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
}

static int
check_multiple_option_occurrences(const char *prog_name, unsigned int option_given, unsigned int min, unsigned int max, const char *option_desc);

int
check_multiple_option_occurrences(const char *prog_name, unsigned int option_given, unsigned int min, unsigned int max, const char *option_desc)
{
  int error_occurred = 0;

  if (option_given && (min > 0 || max > 0))
    {
      if (min > 0 && max > 0)
        {
          if (min == max)
            {
              /* specific occurrences */
              if (option_given != (unsigned int) min)
                {
                  fprintf (stderr, "%s: %s option occurrences must be %d\n",
                    prog_name, option_desc, min);
                  error_occurred = 1;
                }
            }
          else if (option_given < (unsigned int) min
                || option_given > (unsigned int) max)
            {
              /* range occurrences */
              fprintf (stderr, "%s: %s option occurrences must be between %d and %d\n",
                prog_name, option_desc, min, max);
              error_occurred = 1;
            }
        }
      else if (min > 0)
        {
          /* at least check */
          if (option_given < min)
            {
              fprintf (stderr, "%s: %s option occurrences must be at least %d\n",
                prog_name, option_desc, min);
              error_occurred = 1;
            }
        }
      else if (max > 0)
        {
          /* at most check */
          if (option_given > max)
            {
              fprintf (stderr, "%s: %s option occurrences must be at most %d\n",
                prog_name, option_desc, max);
              error_occurred = 1;
            }
        }
    }
    
  return error_occurred;
}
int
cmdline_parser (int argc, char **argv, struct gengetopt_args_info *args_info)
{
  return cmdline_parser2 (argc, argv, args_info, 0, 1, 1);
}

int
cmdline_parser_ext (int argc, char **argv, struct gengetopt_args_info *args_info,
                   struct cmdline_parser_params *params)
{
  int result;
  result = cmdline_parser_internal (argc, argv, args_info, params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser2 (int argc, char **argv, struct gengetopt_args_info *args_info, int override, int initialize, int check_required)
{
  int result;
  struct cmdline_parser_params params;
  
  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;

  result = cmdline_parser_internal (argc, argv, args_info, &params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required (struct gengetopt_args_info *args_info, const char *prog_name)
{
  int result = EXIT_SUCCESS;

  if (cmdline_parser_required2(args_info, prog_name, 0) > 0)
    result = EXIT_FAILURE;

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required2 (struct gengetopt_args_info *args_info, const char *prog_name, const char *additional_error)
{
  int error_occurred = 0;
  FIX_UNUSED (additional_error);

  /* checks for required options */
  if (! args_info->firmware_version_given)
    {
      fprintf (stderr, "%s: '--firmware-version' ('-f') option required%s\n", prog_name, (additional_error ? additional_error : ""));
      error_occurred = 1;
    }
  
  if (check_multiple_option_occurrences(prog_name, args_info->v_given, args_info->v_min, args_info->v_max, "'--v' ('-v')"))
     error_occurred = 1;
  
  
  /* checks for dependences among options */

  return error_occurred;
}


static char *package_name = 0;

/**
 * @brief updates an option
 * @param field the generic pointer to the field to update
 * @param orig_field the pointer to the orig field
 * @param field_given the pointer to the number of occurrence of this option
 * @param prev_given the pointer to the number of occurrence already seen
 * @param value the argument for this option (if null no arg was specified)
 * @param possible_values the possible values for this option (if specified)
 * @param default_value the default value (in case the option only accepts fixed values)
 * @param arg_type the type of this option
 * @param check_ambiguity @see cmdline_parser_params.check_ambiguity
 * @param override @see cmdline_parser_params.override
 * @param no_free whether to free a possible previous value
 * @param multiple_option whether this is a multiple option
 * @param long_opt the corresponding long option
 * @param short_opt the corresponding short option (or '-' if none)
 * @param additional_error possible further error specification
 */
static
int update_arg(void *field, char **orig_field,
               unsigned int *field_given, unsigned int *prev_given, 
               char *value, const char *possible_values[],
               const char *default_value,
               cmdline_parser_arg_type arg_type,
               int check_ambiguity, int override,
               int no_free, int multiple_option,
               const char *long_opt, char short_opt,
               const char *additional_error)
{
  char *stop_char = 0;
  const char *val = value;
  int found;
  char **string_field;
  FIX_UNUSED (field);

  stop_char = 0;
  found = 0;

  if (!multiple_option && prev_given && (*prev_given || (check_ambiguity && *field_given)))
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: `--%s' (`-%c') option given more than once%s\n", 
               package_name, long_opt, short_opt,
               (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: `--%s' option given more than once%s\n", 
               package_name, long_opt,
               (additional_error ? additional_error : ""));
      return 1; /* failure */
    }

  if (possible_values && (found = check_possible_values((value ? value : default_value), possible_values)) < 0)
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: %s argument, \"%s\", for option `--%s' (`-%c')%s\n", 
          package_name, (found == -2) ? "ambiguous" : "invalid", value, long_opt, short_opt,
          (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: %s argument, \"%s\", for option `--%s'%s\n", 
          package_name, (found == -2) ? "ambiguous" : "invalid", value, long_opt,
          (additional_error ? additional_error : ""));
      return 1; /* failure */
    }
    
  if (field_given && *field_given && ! override)
    return 0;
  if (prev_given)
    (*prev_given)++;
  if (field_given)
    (*field_given)++;
  if (possible_values)
    val = possible_values[found];

  switch(arg_type) {
  case ARG_INT:
    if (val) *((int *)field) = strtol (val, &stop_char, 0);
    break;
  case ARG_ENUM:
    if (val) *((int *)field) = found;
    break;
  case ARG_STRING:
    if (val) {
      string_field = (char **)field;
      if (!no_free && *string_field)
        free (*string_field); /* free previous string */
      *string_field = gengetopt_strdup (val);
    }
    break;
  default:
    break;
  };

  /* check numeric conversion */
  switch(arg_type) {
  case ARG_INT:
    if (val && !(stop_char && *stop_char == '\0')) {
      fprintf(stderr, "%s: invalid numeric value: %s\n", package_name, val);
      return 1; /* failure */
    }
    break;
  default:
    ;
  };

  /* store the original value */
  switch(arg_type) {
  case ARG_NO:
    break;
  default:
    if (value && orig_field) {
      if (no_free) {
        *orig_field = value;
      } else {
        if (*orig_field)
          free (*orig_field); /* free previous string */
        *orig_field = gengetopt_strdup (value);
      }
    }
  };

  return 0; /* OK */
}


int
cmdline_parser_internal (
  int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error)
{
  int c;	/* Character of the parsed option.  */

  int error_occurred = 0;
  struct gengetopt_args_info local_args_info;
  
  int override;
  int initialize;
  int check_required;
  int check_ambiguity;
  
  package_name = argv[0];
  
  override = params->override;
  initialize = params->initialize;
  check_required = params->check_required;
  check_ambiguity = params->check_ambiguity;

  if (initialize)
    cmdline_parser_init (args_info);

  cmdline_parser_init (&local_args_info);

  optarg = 0;
  optind = 0;
  opterr = params->print_errors;
  optopt = '?';

  while (1)
    {
      int option_index = 0;

      static struct option long_options[] = {
        { "help",	0, NULL, 'h' },
        { "version",	0, NULL, 'V' },
        { "firmware-version",	1, NULL, 'f' },
        { "pipe-name",	1, NULL, 'p' },
        { "sweep",	1, NULL, 's' },
        { "data-file",	1, NULL, 'd' },
        { "interactive",	0, NULL, 'i' },
        { "hex-output",	0, NULL, 'x' },
        { "v",	0, NULL, 'v' },
        { "verbose",	1, NULL, 0 },
        { "quiet",	0, NULL, 'q' },
        { 0,  0, 0, 0 }
      };

      c = getopt_long (argc, argv, "hVf:p:s:d:ixvq", long_options, &option_index);

      if (c == -1) break;	/* Exit from `while (1)' loop.  */

      switch (c)
        {
        case 'h':	/* Print help and exit.  */
          cmdline_parser_print_help ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'V':	/* Print version and exit.  */
          cmdline_parser_print_version ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'f':	/* TCIP4 firmware version to emulate..  */
        
        
          if (update_arg( (void *)&(args_info->firmware_version_arg), 
               &(args_info->firmware_version_orig), &(args_info->firmware_version_given),
              &(local_args_info.firmware_version_given), optarg, cmdline_parser_firmware_version_values, 0, ARG_INT,
              check_ambiguity, override, 0, 0,
              "firmware-version", 'f',
              additional_error))
            goto failure;
        
          break;
        case 'p':	/* Filename of where a link to the virtual serial port should be made..  */
        
        
          if (update_arg( (void *)&(args_info->pipe_name_arg), 
               &(args_info->pipe_name_orig), &(args_info->pipe_name_given),
              &(local_args_info.pipe_name_given), optarg, 0, 0, ARG_STRING,
              check_ambiguity, override, 0, 0,
              "pipe-name", 'p',
              additional_error))
            goto failure;
        
          break;
        case 's':	/* Set parameter to sweep.  */
        
        
          if (update_arg( (void *)&(args_info->sweep_arg), 
               &(args_info->sweep_orig), &(args_info->sweep_given),
              &(local_args_info.sweep_given), optarg, cmdline_parser_sweep_values, "RPM", ARG_ENUM,
              check_ambiguity, override, 0, 0,
              "sweep", 's',
              additional_error))
            goto failure;
        
          break;
        case 'd':	/* Data file in binary form. File must contain properly sized packets. If this is specified, '--sweep' is ignored..  */
        
        
          if (update_arg( (void *)&(args_info->data_file_arg), 
               &(args_info->data_file_orig), &(args_info->data_file_given),
              &(local_args_info.data_file_given), optarg, 0, 0, ARG_STRING,
              check_ambiguity, override, 0, 0,
              "data-file", 'd',
              additional_error))
            goto failure;
        
          break;
        case 'i':	/* User input required to change output packet..  */
        
        
          if (update_arg( 0 , 
               0 , &(args_info->interactive_given),
              &(local_args_info.interactive_given), optarg, 0, 0, ARG_NO,
              check_ambiguity, override, 0, 0,
              "interactive", 'i',
              additional_error))
            goto failure;
        
          break;
        case 'x':	/* Output a HEX representation of each packet sent..  */
        
        
          if (update_arg( 0 , 
               0 , &(args_info->hex_output_given),
              &(local_args_info.hex_output_given), optarg, 0, 0, ARG_NO,
              check_ambiguity, override, 0, 0,
              "hex-output", 'x',
              additional_error))
            goto failure;
        
          break;
        case 'v':	/* Verbose output. Specify multiple times for increasing verbosity..  */
        
          local_args_info.v_given++;
        
          break;
        case 'q':	/* Suppress output. Same as --verbose=NONE (Overrides both -v and --verbose).  */
        
        
          if (update_arg( 0 , 
               0 , &(args_info->quiet_given),
              &(local_args_info.quiet_given), optarg, 0, 0, ARG_NO,
              check_ambiguity, override, 0, 0,
              "quiet", 'q',
              additional_error))
            goto failure;
        
          break;

        case 0:	/* Long option with no short option */
          /* Set level of verbosity explicitly. 
          (Overrides -v)
.  */
          if (strcmp (long_options[option_index].name, "verbose") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->verbose_arg), 
                 &(args_info->verbose_orig), &(args_info->verbose_given),
                &(local_args_info.verbose_given), optarg, cmdline_parser_verbose_values, "DEBUG", ARG_ENUM,
                check_ambiguity, override, 0, 0,
                "verbose", '-',
                additional_error))
              goto failure;
          
          }
          
          break;
        case '?':	/* Invalid option.  */
          /* `getopt_long' already printed an error message.  */
          goto failure;

        default:	/* bug: option not considered.  */
          fprintf (stderr, "%s: option unknown: %c%s\n", CMDLINE_PARSER_PACKAGE, c, (additional_error ? additional_error : ""));
          abort ();
        } /* switch */
    } /* while */



  args_info->v_given += local_args_info.v_given;
  local_args_info.v_given = 0;
  
  if (check_required)
    {
      error_occurred += cmdline_parser_required2 (args_info, argv[0], additional_error);
    }

  cmdline_parser_release (&local_args_info);

  if ( error_occurred )
    return (EXIT_FAILURE);

  return 0;

failure:
  
  cmdline_parser_release (&local_args_info);
  return (EXIT_FAILURE);
}