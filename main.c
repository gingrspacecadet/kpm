#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE       512
#define CONFIG_DIR     "/etc/kpm/kpm.conf"
#define INSTALLED_LIST "/mnt/us/kpm/packages.list"
#define MENU_JSON_PATH "/mnt/us/extensions/kpm/menu.json"
#define VERSION "1.1.0"

static char INSTALL_DIR[MAX_LINE];
static char MIRRORS_CONF[MAX_LINE];
static char TMP_LIST_FILE[MAX_LINE];

void load_config(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Failed to open config file: %s: %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0';

        if (line[0]=='\0' || line[0]=='#') continue;
        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0';
        char *key = line;
        char *val = eq + 1;

        if (strcmp(key, "INSTALL_DIR") == 0) {
            strncpy(INSTALL_DIR, val, MAX_LINE-1);
        } else if (strcmp(key, "MIRRORS_CONF") == 0) {
            strncpy(MIRRORS_CONF, val, MAX_LINE-1);
        } else if (strcmp(key, "TMP_LIST_FILE") == 0) {
            strncpy(TMP_LIST_FILE, val, MAX_LINE-1);
        }
    }
    fclose(file);
}

char *format_string(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    int length = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    char *str = malloc(length + 1);
    if (!str) return NULL;

    va_start(args, fmt);
    vsnprintf(str, length + 1, fmt, args);
    va_end(args);

    return str;
}

int download(const char *url, const char *outpath) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
             "curl -fsSL \"%s\" -o \"%s\"", url, outpath);
    return system(cmd);
}

int find_in_list(const char *listpath, const char *pkg) {
    FILE *f = fopen(listpath, "r");
    if (!f) {
        fprintf(stderr, "ERROR: couldn't open package list '%s': %s\n",
                listpath, strerror(errno));
        return 0;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        char *p = line + strlen(line) - 1;
        while (p >= line && (*p == '\n' || *p == '\r')) {
            *p-- = '\0';
        }

        if (strcmp(line, pkg) == 0) {
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

static char **collect_lines(const char *cmd) {
    FILE *fp = popen(cmd, "r");
    if (!fp) return NULL;

    char **lines = NULL;
    size_t count = 0;
    char buffer[MAX_LINE];

    while (fgets(buffer, sizeof(buffer), fp)) {
        buffer[strcspn(buffer, "\r\n")] = '\0';
        if (buffer[0] == '\0') continue;

        char *dup = strdup(buffer);
        if (!dup) break;
        char **tmp = realloc(lines, (count + 2) * sizeof(char *));
        if (!tmp) {
            free(dup);
            break;
        }
        lines = tmp;
        lines[count++] = dup;
        lines[count] = NULL;
    }
    pclose(fp);
    return lines;
}

static void free_lines(char **lines) {
    if (!lines) return;
    for (size_t i = 0; lines[i]; i++) {
        free(lines[i]);
    }
    free(lines);
}

static void trim_leading_whitespace(char *str) {
    char *p = str;
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
    if (p != str) {
        memmove(str, p, strlen(p) + 1);
    }
}

static int in_list(char **haystack, const char *needle) {
    if (!haystack || !needle) return 0;
    for (size_t i = 0; haystack[i]; i++) {
        if (strcmp(haystack[i], needle) == 0) {
            return 1;
        }
    }
    return 0;
}

static void update_kual_menu(void) {
    char **remote = collect_lines("kpm -Qr");
    char **local  = collect_lines("kpm -Ql");

    if (!remote || !local) {
        free_lines(remote);
        free_lines(local);
        return;
    }

    FILE *jf = fopen(MENU_JSON_PATH, "w");
    if (!jf) {
        fprintf(stderr, "ERROR: Could not open %s for writing: %s\n",
                MENU_JSON_PATH, strerror(errno));
        free_lines(remote);
        free_lines(local);
        return;
    }

    fprintf(jf,
        "{\n"
        "	\"items\": [\n"
        "	{\n"
        "		\"name\": \"KPM\",\n"
        "		\"priority\": 0,\n"
        "		\"items\": [\n");

    fprintf(jf,
        "		{\n"
        "			\"name\": \"Install Package\",\n"
        "			\"priority\": 1,\n"
        "			\"items\": [\n");

    int first_install = 1;
    for (size_t i = 0; remote[i]; i++) {
        trim_leading_whitespace(remote[i]);
        if (strstr(remote[i], "https://")) {
            continue;
        }
        if (!in_list(local, remote[i])) {
            if (!first_install) {
                fprintf(jf, ",\n");
            }
            first_install = 0;
            fprintf(jf,
                "			{\n"
                "				\"name\": \"%s\",\n"
                "				\"priority\": 2,\n"
                "				\"action\": \"/mnt/us/extensions/kterm/bin/kterm\",\n"
                "				\"params\": \"-e \'kpm -S %s\'\",\n"
                "				\"exitmenu\": false,\n"
                "				\"refresh\": true,\n"
                "				\"status\": false,\n"
                "				\"internal\": \"status Installing %s...\"\n"
                "			}",
                remote[i], remote[i], remote[i]);
        }
    }
    fprintf(jf,
        "\n"
        "			]\n"
        "		},\n");

    fprintf(jf,
        "		{\n"
        "			\"name\": \"Remove Package\",\n"
        "			\"priority\": 2,\n"
        "			\"items\": [\n");

    int first_remove = 1;
    for (size_t i = 0; local[i]; i++) {
        trim_leading_whitespace(local[i]);
        if (!first_remove) {
            fprintf(jf, ",\n");
        }
        first_remove = 0;
        fprintf(jf,
            "			{\n"
            "				\"name\": \"%s\",\n"
            "				\"priority\": 2,\n"
            "				\"action\": \"/mnt/us/extensions/kterm/bin/kterm\",\n"
            "				\"params\": \"-e \'kpm -R %s\'\",\n"
            "				\"exitmenu\": false,\n"
            "				\"refresh\": true,\n"
            "				\"status\": false,\n"
            "				\"internal\": \"status Removing %s...\"\n"
            "			}",
            local[i], local[i], local[i]);
    }
    fprintf(jf,
        "\n"
        "			]\n"
        "		},\n");

    fprintf(jf,
        "		{\n"
        "			\"name\": \"Configure KPM\",\n"
        "			\"priority\": 3,\n"
        "			\"action\": \"/mnt/us/extensions/kterm/bin/kterm\",\n"
        "			\"params\": \"-e \'$EDITOR /etc/kpm/kpm.conf\'\", \n"
        "			\"exitmenu\": false,\n"
        "			\"internal\": \"status Editing kpm.conf…\"\n"
        "		}\n");

    fprintf(jf,
        "			]\n"
        "		}\n"
        "	]\n"
        "}\n");

    fclose(jf);
    free_lines(remote);
    free_lines(local);
}

static void get_pkg_name(const char *path, char *out_pkg, size_t sz) {
    const char *base = strrchr(path, '/');
    base = base ? base + 1 : path;
    char tmp[MAX_LINE];
    strncpy(tmp, base, MAX_LINE-1);
    tmp[MAX_LINE-1] = '\0';
    const char *sufs[] = {".tar.gz", ".tar.xz",  ".zip", NULL};
    for (int i = 0; sufs[i]; i++) {
        size_t len = strlen(sufs[i]);
        size_t tlen = strlen(tmp);
        if (tlen > len && strcmp(tmp + tlen - len, sufs[i]) == 0) {
            tmp[tlen - len] = '\0';
            break;
        }
    }
    strncpy(out_pkg, tmp, sz-1);
    out_pkg[sz-1] = '\0';
}

static int ensure_dir(const char *path, mode_t mode) {
    if (mkdir(path, mode) == 0 || errno == EEXIST) {
        return 0;
    } else {
        perror(path);
        return -1;
    }
}

static int run_cmd(char *const argv[]) {
    system("{\nmntroot rw\n} &> /dev/null");
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    if (pid == 0) {
        execvp(argv[0], argv);
        perror("execvp");
        _exit(127);
    }
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return -1;
    }
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        return 0;
    } else {
        fprintf(stderr, "%s failed with exit code %d\n", argv[0], WEXITSTATUS(status));
        return -1;
    }
}

static int run_cmd_str(const char *cmd) {
    char *const argv[] = {"sh", "-c", (char *)cmd, NULL};
    return run_cmd(argv);
}

int fetch_package(const char *mirror_fmt, const char *pkg) {
    char url[MAX_LINE * 2], outpath[MAX_LINE], pkgdir[MAX_LINE];
    const char *marker = "{pkg}";
    size_t mlen = strlen(marker), plen = strlen(pkg);

    const char *s = mirror_fmt;
    char *d = url;
    while (*s && (d - url) < (int)sizeof(url) - 1) {
        if (strncmp(s, marker, mlen) == 0) {
            if ((d - url) + plen >= sizeof(url) - 1) break;
            memcpy(d, pkg, plen);
            d += plen;
            s += mlen;
        } else {
            *d++ = *s++;
        }
    }
    *d = '\0';

    const char *slash = strrchr(url, '/');
    const char *dot = slash ? strrchr(slash, '.') : NULL;
    const char *suffix = dot ? dot : "";

    if (strcmp(pkg, "kpm") == 0) {
        char *last_dot = strrchr(url, '.');
        if (last_dot && last_dot > strrchr(url, '/')) {
            *last_dot = '\0';
        }
    }

    snprintf(outpath, sizeof(outpath), "%s/%s%s", INSTALL_DIR, pkg, suffix);
    snprintf(pkgdir,  sizeof(pkgdir),  "%s/%s", INSTALL_DIR, pkg);

    if (ensure_dir(INSTALL_DIR, 0755)) {
        return 0;
    }

    if (strcmp(pkg, "kpm") != 0) {
        if (ensure_dir(pkgdir, 0755)) {
            return 0;
        }
    }

    printf("Downloading %s -> %s\n", url, outpath);
    if (download(url, outpath) != 0) {
        fprintf(stderr, "Download failed\n");
        return 0;
    }

    if (strcmp(pkg, "kpm") != 0) {
        if (strstr(suffix, ".zip")) {
            char *const args[] = { "unzip", "-o", outpath, "-d", pkgdir, NULL };
            printf("Extracting ZIP...\n");
            if (run_cmd(args) < 0) return 0;
        }
        else if (strstr(suffix, ".tar.gz")) {
            char *const args[] = { "tar", "-xzf", outpath, "-C", pkgdir, NULL };
            printf("Extracting tar.gz...\n");
            if (run_cmd(args) < 0) return 0;
        }
        else if (strstr(suffix, ".tar.xz")) {
            char *const args[] = { "tar", "-xJf", outpath, "-C", pkgdir, NULL };
            printf("Extracting tar.xz...\n");
            if (run_cmd(args) < 0) return 0;
        }
        else {
            fprintf(stderr, "Unknown archive format: %s\n", suffix);
            return 0;
        }

        char script[MAX_LINE];
        snprintf(script, sizeof(script), "%s/install.sh", pkgdir);
        if (access(script, F_OK) != 0) {
            fprintf(stderr, "Error: install.sh not found in %s\n", pkgdir);
            return 0;
        }

        if (chmod(script, 0755) != 0) {
            perror("chmod install.sh");
            return 0;
        }

        printf("Running install script: %s\n", script);
        char *const runargs[] = { script, NULL };
        if (run_cmd(runargs) < 0) {
            fprintf(stderr, "install.sh failed\n");
            return 0;
        }
    }

    return 1;
}

int install_from_mirrors(const char *pkg) {    
    FILE *mf = fopen(MIRRORS_CONF, "r");    
    if (!mf) {
        fprintf(stderr, "Cannot open %s: %s\n",
                MIRRORS_CONF, strerror(errno));
        return 0;
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), mf)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        char list_url[MAX_LINE], pkg_fmt[MAX_LINE];
        if (sscanf(line, "%s %s", list_url, pkg_fmt) != 2) continue;

        printf("Trying mirror: %s\n", list_url);
        if (download(list_url, TMP_LIST_FILE) != 0) {
            fprintf(stderr, "  failed to fetch list\n");
            continue;
        }
        if (find_in_list(TMP_LIST_FILE, pkg)) {
            printf("  %s found; fetching package...\n", pkg);
            fclose(mf);
            int result = fetch_package(pkg_fmt, pkg);

            if (result && strcmp(pkg, "kpm") == 0) {
                char *const cp_argv[] = {
                    "cp",
                    "/mnt/us/kpm/packages/kpm",
                    "/usr/local/bin/kpm",
                    NULL
                };
                printf("Replacing /usr/local/bin/kpm with the new binary...\n");
                if (run_cmd(cp_argv) != 0) {
                    fprintf(stderr, "Failed to replace kpm binary\n");
                    return 0;
                }
            }

            return result;
        } else {
            printf("  %s not here; next mirror\n", pkg);
        }
    }
    fclose(mf);
    fprintf(stderr, "Package %s not found on any mirror\n", pkg);
    return 0;
}

int install_package(const char *pkg) {
    if (find_in_list(INSTALLED_LIST, pkg)) {
        printf("Package '%s' already installed.\n", pkg);
        return 1;
    }
    if (!install_from_mirrors(pkg)) {
        return 0;
    }

    printf("1fully installed %s\n", pkg);
    
    if (strcmp(pkg, "kpm") != 0) {
        FILE *f = fopen(INSTALLED_LIST, "a");
        if (f) {
            fprintf(f, "%s\n", pkg);
            fclose(f);
        }
    }
    
    if (strcmp(collect_lines("kpm -Ql kpmgui")[0], format_string("Package 'kpmgui' is installed (listed in %s).", INSTALL_DIR)) == 0) {
        update_kual_menu();
    }
    return 1;
}

int install_from_file(const char *pkg_path) {
    char pkg[MAX_LINE];
    get_pkg_name(pkg_path, pkg, sizeof(pkg));

    if (find_in_list(INSTALLED_LIST, pkg)) {
        printf("Package '%s' already installed.\n", pkg);
        return 1;
    }

    char outpath[MAX_LINE], pkgdir[MAX_LINE];
    snprintf(outpath, sizeof(outpath), "%s/%s", INSTALL_DIR, pkg);
    snprintf(pkgdir, sizeof(pkgdir),  "%s/%s", INSTALL_DIR, pkg);

    if (ensure_dir(INSTALL_DIR, 0755)) return 0;
    if (ensure_dir(pkgdir, 0755)) return 0;

    {
        char *const cp_argv[] = { "cp", pkg_path, outpath, NULL };
        printf("Copying %s -> %s\n", pkg_path, outpath);
        if (run_cmd(cp_argv) != 0) {
            fprintf(stderr, "Failed to copy package file\n");
            return 0;
        }
    }

    if (strstr(pkg_path, ".zip")) {
        char *const args[] = { "unzip", "-o", outpath, "-d", pkgdir, NULL };
        printf("Extracting ZIP...\n");
        if (run_cmd(args) < 0) return 0;
    } else if (strstr(pkg_path, ".tar.gz")) {
        char *const args[] = { "tar", "-xzf", outpath, "-C", pkgdir, NULL };
        printf("Extracting tar.gz...\n");
        if (run_cmd(args) < 0) return 0;
    } else if (strstr(pkg_path, ".tar.xz")) {
        char *const args[] = { "tar", "-xJf", outpath, "-C", pkgdir, NULL };
        printf("Extracting tar.xz...\n");
        if (run_cmd(args) < 0) return 0;
    }

    {
        char script[MAX_LINE];
        snprintf(script, sizeof(script), "%s/install.sh", pkgdir);
        if (access(script, F_OK) == 0) {
            if (chmod(script, 0755) != 0) {
                perror("chmod install.sh");
                return 0;
            }
            printf("Running install script: %s\n", script);
            char *const runargs[] = { script, NULL };
            if (run_cmd(runargs) < 0) {
                fprintf(stderr, "install.sh failed\n");
                return 0;
            }
        }
    }

    if (strcmp(pkg, "kpm") != 0) {
        FILE *f = fopen(INSTALLED_LIST, "a");
        if (f) {
            fprintf(f, "%s\n", pkg);
            fclose(f);
        }
    }

    printf("1fully installed '%s' from local file\n", pkg);
    return 1;
}

int uninstall_package(const char *pkg) {
    char pkgdir[MAX_LINE];
    snprintf(pkgdir, sizeof(pkgdir), "%s/%s", INSTALL_DIR, pkg);

    if (strcmp(pkg, "kpm") == 0) {
        run_cmd_str("rm -f /usr/local/bin/kpm");
    }

    char script[MAX_LINE];
    snprintf(script, sizeof(script), "%s/uninstall.sh", pkgdir);
    if (access(script, F_OK) != 0) {
        fprintf(stderr, "Error: uninstall.sh not found in %s\n", pkgdir);
        return 0;
    }
    if (chmod(script, 0755) != 0) {
        perror("chmod uninstall.sh");
        return 0;
    }
    printf("Running uninstall script: %s\n", script);
    char *const runargs[] = { script, NULL };
    if (run_cmd(runargs) < 0) {
        fprintf(stderr, "uninstall.sh failed\n");
        return 0;
    }

    FILE *in = fopen(INSTALLED_LIST, "r");
    if (!in) {
        perror("fopen INSTALLED_LIST");
        return 0;
    }
    char tmp[MAX_LINE];
    snprintf(tmp, sizeof(tmp), "%s.tmp", INSTALLED_LIST);
    FILE *out = fopen(tmp, "w");
    if (!out) {
        perror("fopen tmp list");
        fclose(in);
        return 0;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), in)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, pkg) != 0) {
            fprintf(out, "%s\n", line);
        }
    }
    fclose(in);
    fclose(out);

    if (rename(tmp, INSTALLED_LIST) != 0) {
        perror("rename list");
        return 0;
    }

    if (strcmp(collect_lines("kpm -Ql kpmgui")[0], format_string("Package 'kpmgui' is installed (listed in %s).", INSTALL_DIR)) == 0) {
        update_kual_menu();
    }
    printf("Package '%s' removed and list updated.\n", pkg);
    return 1;
}

int x(char subop, const char *pkg) {
    switch (subop) {
        case 'i':
            printf("%s", INSTALL_DIR);
            break;
        case 'm':
            printf("%s", MIRRORS_CONF);
            break;
        case 't':
            printf("%s", TMP_LIST_FILE);
            break;
    }
    return 0;
}

int query_package_local(const char *pkg) {
    if (pkg != NULL) {
        if (find_in_list(INSTALLED_LIST, pkg)) {
            printf("Package '%s' is installed (listed in %s).\n",
                    pkg, INSTALLED_LIST);
            return 0;
        } else {
            printf("Package '%s' is not installed (listed in %s).\n",
                    pkg, INSTALLED_LIST);
        }
    } else {
        FILE *f = fopen(INSTALLED_LIST, "r");
        if (!f) {
            fprintf(stderr, "Failed to open installed list '%s': %s\n",
                    INSTALLED_LIST, strerror(errno));
            return 1;
        }

        printf("Installed packages:\n");
        char line[MAX_LINE];
        while (fgets(line, sizeof(line), f)) {
            printf("  %s", line);
        }

        fclose(f);
    }
}

int query_package_remote(const char *pkg) {
    FILE *mf = fopen(MIRRORS_CONF, "r");
    if (!mf) {
        fprintf(stderr, "Cannot open mirrors config '%s': %s\n",
                MIRRORS_CONF, strerror(errno));
        return 1;
    }

    char line[MAX_LINE];
    int found_any = 0;
    while (fgets(line, sizeof(line), mf)) {
        char list_url[MAX_LINE], pkg_fmt[MAX_LINE];
        if (sscanf(line, "%s %s", list_url, pkg_fmt) != 2) {
            continue;
        }

        printf("Checking mirror: %s\n", list_url);
        if (download(list_url, TMP_LIST_FILE) != 0) {
            fprintf(stderr, "  [!] Failed to download list from %s\n", list_url);
            continue;
        }
        
        if (pkg != NULL) {
            if (find_in_list(TMP_LIST_FILE, pkg)) {
                printf("  [v] Package '%s' is available on %s\n", pkg, list_url);
                found_any = 1;
            } else {
                printf("  [x] Package '%s' not on %s\n", pkg, list_url);
            }
        } else {
            printf("Packages on %s:\n", list_url);

            FILE *f = fopen(TMP_LIST_FILE, "r");
            if (!f) {
                fprintf(stderr, "  Failed to open list '%s': %s\n",
                        TMP_LIST_FILE, strerror(errno));
            } else {
                char line[MAX_LINE];
                while (fgets(line, sizeof(line), f)) {
                    printf("  %s", line);
                }
                fclose(f);
            }
        }
    }
    
    fclose(mf);
    
    if (!found_any && pkg != NULL) {
        printf("Package '%s' is not available on any configured mirror.\n", pkg);
        return 1;
    }
    
    return 0;
}

void help(char op) {
    switch (op) {
        case 'Q':
            fprintf(stderr,
                "Usage:\n"
                "  -Ql [<pkg>]   list local packages (or check one)\n"
                "  -Qr [<pkg>]   list remote packages (or check one)\n");
            break;
        case 'C':
            fprintf(stderr,
                "Usage:\n"
                "  -Cc           open config file\n"
                "  -Cm           open mirror config file\n");
            break;
        case 'X':
            fprintf(stderr,
                "Usage:\n"
                "  -Xi           print INSTALL_DIR\n"
                "  -Xm           print MIRRORS_CONF\n"
                "  -Xt           print TEMP_LIST_FILE\n");
            break;
        default:
            fprintf(stderr,
                "Usage:\n"
                "  kpm [OPTIONS]\n\n"
                "Short options:\n"
                "  -S <pkg>      install package\n"
                "  -R <pkg>      remove package\n"
                "  -Q <pkg>      query\n"
                "  -U <pkg>      install a package from local zip\n"
                "  -C            configuring\n"
                "  -X            prints config variables\n\n"
                "Long options:\n"
                "  --help        show this help and exit\n"
                "  --version     show version and exit\n");
            break;
    }
    
    exit(1);
}

int do_install(const char *pkg) {
    return install_package(pkg) ? 0 : 1;
}
int do_install_local(const char *pkg) {
    return install_from_file(pkg) ? 0 : 1;
}
int do_remove(const char *pkg) {
    return uninstall_package(pkg) ? 0 : 1;
}
int do_query(char subop, const char *pkg) {
    switch (subop) {
        case 'l':
            return query_package_local(pkg) ? 0 : 1; 
        case 'r':
            return query_package_remote(pkg) ? 0 : 1;
        case 'h':
            help('Q');
            return 0; 
    }
}
int do_config(char subop, const char *unused_pkg) {
    const char *path;

    switch (subop) {
      case 'c':
        path = CONFIG_DIR;
        break;
      case 'm':
        path = MIRRORS_CONF;
        break;
      default:
        fprintf(stderr, "Unknown config sub-option '%c'\n", subop);
        return 1;
    }

    const char *editor = getenv("EDITOR");
    if (!editor || !*editor) {
        editor = "vi";
    }

    char *const argv[] = {
        (char *)editor,
        (char *)path,
        NULL
    };

    if (run_cmd(argv) < 0) {
        fprintf(stderr, "Failed to launch editor '%s' on %s\n", editor, path);
        return 1;
    }

    return 0;
}

int do_x(char subop, const char *pkg) {
    return x(subop, pkg) ? 0 : 1;
}

int main(int argc, char *argv[]) {
    load_config(CONFIG_DIR);

    if (argc == 2) {
        if (strcmp(argv[1], "--version") == 0) {
            printf("kpm version %s\n", VERSION);
            return 0;
        }
        if (strcmp(argv[1], "--help") == 0) {
            help('\0');
            return 0;
        }
    }

    if (argc < 2 || argv[1][0] != '-' || argv[1][1] == '\0') {
        help('\0');
    }

    char op    = argv[1][1];
    char subop = argv[1][2];
    const char *pkg = argv[2];

    switch (op) {
        case 'S':
            if (subop || argc!=3) help(op);
            return do_install(pkg);
        case 'R':
            if (subop || argc!=3) help(op);
            return do_remove(pkg);
        case 'Q':
            if (!subop) help(op);
            return do_query(subop, pkg);
        case 'U':
            if (subop || argc!=3) help(op);
            return do_install_local(pkg);
        case 'C':
            if (!subop) help(op);
            return do_config(subop, pkg);
        case 'X':
            if (!subop) help(op);
            return do_x(subop, pkg);
        default:
            help(op);
    }
    return 1;
}
