#ifndef _SALLANG_PARSER_ERROR_H
#define _SALLANG_PARSER_ERROR_H

void print_fatal(const char *msg, ...);
void print_error(const char *msg, ...);
void print_warning(const char *msg, ...);

void trigger_fatal(int lineno, const char *msg, ...);
void trigger_error(int lineno, const char *msg, ...);
void trigger_warning(int lineno, const char *msg, ...);

#endif
