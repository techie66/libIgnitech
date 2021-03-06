/** @file cmdline.h
 *  @brief The header file for the command line option parser
 *  generated by GNU Gengetopt version 2.22.6
 *  http://www.gnu.org/software/gengetopt.
 *  DO NOT modify this file, since it can be overwritten
 *  @author GNU Gengetopt by Lorenzo Bettini */

#ifndef CMDLINE_H
#define CMDLINE_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for FILE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
/** @brief the program name (used for printing errors) */
#define CMDLINE_PARSER_PACKAGE "TCIP4 Emulator"
#endif

#ifndef CMDLINE_PARSER_PACKAGE_NAME
/** @brief the complete program name (used for help and version) */
#define CMDLINE_PARSER_PACKAGE_NAME "TCIP4 Emulator"
#endif

#ifndef CMDLINE_PARSER_VERSION
/** @brief the program version */
#define CMDLINE_PARSER_VERSION VERSION
#endif

enum enum_verbose { verbose__NULL = -1, verbose_arg_NONE = 0, verbose_arg_ERROR, verbose_arg_WARN, verbose_arg_INFO, verbose_arg_DEBUG };
enum enum_sweep { sweep__NULL = -1, sweep_arg_RPM = 0, sweep_arg_SENSOR_VALUE, sweep_arg_SENSORMV, sweep_arg_SENSOR_TYPE, sweep_arg_BATTMV, sweep_arg_PROGRAMMINGS, sweep_arg_SERVO_MEASURED, sweep_arg_SERVO_REQUESTED, sweep_arg_NUM_CYLINDERS, sweep_arg_CH1_ADVANCE, sweep_arg_CH2_ADVANCE, sweep_arg_CH3_ADVANCE, sweep_arg_CH4_ADVANCE, sweep_arg_ALL_ADVANCE, sweep_arg_CH1_MAXADVANCE, sweep_arg_CH2_MAXADVANCE, sweep_arg_CH3_MAXADVANCE, sweep_arg_CH4_MAXADVANCE, sweep_arg_ALL_MAXADVANCE, sweep_arg_DWELL_OPT, sweep_arg_DWELL, sweep_arg_RUNTIME, sweep_arg_PROP1, sweep_arg_PROP2, sweep_arg_PROP3, sweep_arg_PROP4, sweep_arg_RESPONSE_NUMBER, sweep_arg_LIMITER, sweep_arg_START_LIMITER, sweep_arg_RETARD, sweep_arg_CLUTCH_MASTER, sweep_arg_POWER_OUT, sweep_arg_FLAGS_V88_90, sweep_arg_FLAGS_V88_91, sweep_arg_FLAGS_V96_140, sweep_arg_FLAGS_V96_141, sweep_arg_FLAGS_V96_142, sweep_arg_FLAGS_V96_143, sweep_arg_FLAGS_V96_144, sweep_arg_FLAGS_V96_145 };

/** @brief Where the command line options are stored */
struct gengetopt_args_info
{
  const char *help_help; /**< @brief Print help and exit help description.  */
  const char *detailed_help_help; /**< @brief Print help, including all details and hidden options, and exit help description.  */
  const char *version_help; /**< @brief Print version and exit help description.  */
  int firmware_version_arg;	/**< @brief TCIP4 firmware version to emulate..  */
  char * firmware_version_orig;	/**< @brief TCIP4 firmware version to emulate. original value given at command line.  */
  const char *firmware_version_help; /**< @brief TCIP4 firmware version to emulate. help description.  */
  char * pipe_name_arg;	/**< @brief Filename of where a link to the virtual serial port should be made..  */
  char * pipe_name_orig;	/**< @brief Filename of where a link to the virtual serial port should be made. original value given at command line.  */
  const char *pipe_name_help; /**< @brief Filename of where a link to the virtual serial port should be made. help description.  */
  char * data_file_arg;	/**< @brief Data file in binary form. File must contain properly sized packets. If this is specified, '--sweep' is ignored..  */
  char * data_file_orig;	/**< @brief Data file in binary form. File must contain properly sized packets. If this is specified, '--sweep' is ignored. original value given at command line.  */
  const char *data_file_help; /**< @brief Data file in binary form. File must contain properly sized packets. If this is specified, '--sweep' is ignored. help description.  */
  const char *interactive_help; /**< @brief User input required to change output packet. help description.  */
  const char *hex_output_help; /**< @brief Output a HEX representation of each packet sent. help description.  */
  unsigned int v_min; /**< @brief Verbose output. Specify multiple times for increasing verbosity.'s minimum occurreces */
  unsigned int v_max; /**< @brief Verbose output. Specify multiple times for increasing verbosity.'s maximum occurreces */
  const char *v_help; /**< @brief Verbose output. Specify multiple times for increasing verbosity. help description.  */
  enum enum_verbose verbose_arg;	/**< @brief Set level of verbosity explicitly. 
  (Overrides -v)
 (default='DEBUG').  */
  char * verbose_orig;	/**< @brief Set level of verbosity explicitly. 
  (Overrides -v)
 original value given at command line.  */
  const char *verbose_help; /**< @brief Set level of verbosity explicitly. 
  (Overrides -v)
 help description.  */
  const char *quiet_help; /**< @brief Suppress output. Same as --verbose=NONE (Overrides both -v and --verbose) help description.  */
  enum enum_sweep sweep_arg;	/**< @brief Set parameter to sweep (default='RPM').  */
  char * sweep_orig;	/**< @brief Set parameter to sweep original value given at command line.  */
  const char *sweep_help; /**< @brief Set parameter to sweep help description.  */
  
  unsigned int help_given ;	/**< @brief Whether help was given.  */
  unsigned int detailed_help_given ;	/**< @brief Whether detailed-help was given.  */
  unsigned int version_given ;	/**< @brief Whether version was given.  */
  unsigned int firmware_version_given ;	/**< @brief Whether firmware-version was given.  */
  unsigned int pipe_name_given ;	/**< @brief Whether pipe-name was given.  */
  unsigned int data_file_given ;	/**< @brief Whether data-file was given.  */
  unsigned int interactive_given ;	/**< @brief Whether interactive was given.  */
  unsigned int hex_output_given ;	/**< @brief Whether hex-output was given.  */
  unsigned int v_given ;	/**< @brief Whether v was given.  */
  unsigned int verbose_given ;	/**< @brief Whether verbose was given.  */
  unsigned int quiet_given ;	/**< @brief Whether quiet was given.  */
  unsigned int sweep_given ;	/**< @brief Whether sweep was given.  */

} ;

/** @brief The additional parameters to pass to parser functions */
struct cmdline_parser_params
{
  int override; /**< @brief whether to override possibly already present options (default 0) */
  int initialize; /**< @brief whether to initialize the option structure gengetopt_args_info (default 1) */
  int check_required; /**< @brief whether to check that all required options were provided (default 1) */
  int check_ambiguity; /**< @brief whether to check for options already specified in the option structure gengetopt_args_info (default 0) */
  int print_errors; /**< @brief whether getopt_long should print an error message for a bad option (default 1) */
} ;

/** @brief the purpose string of the program */
extern const char *gengetopt_args_info_purpose;
/** @brief the usage string of the program */
extern const char *gengetopt_args_info_usage;
/** @brief the description string of the program */
extern const char *gengetopt_args_info_description;
/** @brief all the lines making the help output */
extern const char *gengetopt_args_info_help[];
/** @brief all the lines making the detailed help output (including hidden options and details) */
extern const char *gengetopt_args_info_detailed_help[];

/**
 * The command line parser
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser (int argc, char **argv,
  struct gengetopt_args_info *args_info);

/**
 * The command line parser (version with additional parameters - deprecated)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_ext() instead
 */
int cmdline_parser2 (int argc, char **argv,
  struct gengetopt_args_info *args_info,
  int override, int initialize, int check_required);

/**
 * The command line parser (version with additional parameters)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_ext (int argc, char **argv,
  struct gengetopt_args_info *args_info,
  struct cmdline_parser_params *params);

/**
 * Save the contents of the option struct into an already open FILE stream.
 * @param outfile the stream where to dump options
 * @param args_info the option struct to dump
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_dump(FILE *outfile,
  struct gengetopt_args_info *args_info);

/**
 * Save the contents of the option struct into a (text) file.
 * This file can be read by the config file parser (if generated by gengetopt)
 * @param filename the file where to save
 * @param args_info the option struct to save
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_file_save(const char *filename,
  struct gengetopt_args_info *args_info);

/**
 * Print the help
 */
void cmdline_parser_print_help(void);
/**
 * Print the detailed help (including hidden options and details)
 */
void cmdline_parser_print_detailed_help(void);
/**
 * Print the version
 */
void cmdline_parser_print_version(void);

/**
 * Initializes all the fields a cmdline_parser_params structure 
 * to their default values
 * @param params the structure to initialize
 */
void cmdline_parser_params_init(struct cmdline_parser_params *params);

/**
 * Allocates dynamically a cmdline_parser_params structure and initializes
 * all its fields to their default values
 * @return the created and initialized cmdline_parser_params structure
 */
struct cmdline_parser_params *cmdline_parser_params_create(void);

/**
 * Initializes the passed gengetopt_args_info structure's fields
 * (also set default values for options that have a default)
 * @param args_info the structure to initialize
 */
void cmdline_parser_init (struct gengetopt_args_info *args_info);
/**
 * Deallocates the string fields of the gengetopt_args_info structure
 * (but does not deallocate the structure itself)
 * @param args_info the structure to deallocate
 */
void cmdline_parser_free (struct gengetopt_args_info *args_info);

/**
 * Checks that all the required options were specified
 * @param args_info the structure to check
 * @param prog_name the name of the program that will be used to print
 *   possible errors
 * @return
 */
int cmdline_parser_required (struct gengetopt_args_info *args_info,
  const char *prog_name);

extern const char *cmdline_parser_firmware_version_values[];  /**< @brief Possible values for firmware-version. */
extern const char *cmdline_parser_verbose_values[];  /**< @brief Possible values for verbose. */
extern const char *cmdline_parser_sweep_values[];  /**< @brief Possible values for sweep. */


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* CMDLINE_H */
