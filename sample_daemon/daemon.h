typedef int (*daemon_main_t)(int argc, char *argv[]);
typedef struct {
    daemon_main_t callback;
    const char *full_name;
    const char *version;
    const char *author;
    const char *mail;
} daemon_configure_t;

extern int daemon_main(const daemon_configure_t *configure, int argc, char *argv[]);
