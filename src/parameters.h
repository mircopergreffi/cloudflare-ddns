
#ifndef PARAMETERS_H
#define PARAMETERS_H

struct Parameters_T
{
	char *token;
    char *zone_id;
    int update_interval;
    char *domain;
    char *bind_template;
    char *bind_template_noproxy;
    int skip_cname;
};
typedef struct Parameters_T Parameters;

extern char * get_env(const char *name);

extern char * get_env_and_print(const char *name);

extern char * str_replace(char *str, const char *pattern, const char *newvalue);

extern char * replace_bind(char *bind_template, const char *ip, const char *domain);

extern Parameters get_parameters();

#endif /* PARAMETERS_H */