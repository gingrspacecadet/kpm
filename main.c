#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 512
#define CMD        "wget -q -O %s \"%s\""
#define CONFIG_DIR "/etc/kpm/kpm.conf"
#define INSTALLED_LIST "/mnt/us/kpm/package_list.conf"

// These will be filled in by load_config()
char INSTALL_DIR[MAX_LINE];
char MIRRORS_CONF[MAX_LINE];
char TMP_LIST_FILE[MAX_LINE];

void load_config(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Failed to open config file: %s: %s\n",
                path, strerror(errno));
        exit(1);
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        // strip CR/LF
        line[strcspn(line, "\r\n")] = '\0';

        // skip empty or comment
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

// Download URL to local path using wget 
int download(const char *url, const char *outpath) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), CMD, outpath, url);
    return system(cmd);
}

// Search `listpath` for a line beginning with `pkg`, return 1 if found 
int find_in_list(const char *listpath, const char *pkg) {
    FILE *f = fopen(listpath, "r");
    if (!f) {
        fprintf(stderr, "ERROR: couldn’t open package list '%s': %s\n",
                listpath, strerror(errno));
        return 0;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        // strip trailing CR/LF
        char *p = line + strlen(line) - 1;
        while (p >= line && (*p == '\n' || *p == '\r')) {
            *p-- = '\0';
        }

        // now line is a clean token; compare exactly
        if (strcmp(line, pkg) == 0) {
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

// Helper: create one directory, ignore EEXIST
static int ensure_dir(const char *path, mode_t mode) {
    if (mkdir(path, mode) == 0 || errno == EEXIST) {
        return 0;
    } else {
        perror(path);
        return -1;
    }
}

// Helper: run a command via fork+execvp and wait for it
static int run_cmd(char *const argv[]) {
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
        fprintf(stderr, "%s failed with exit code %d\n",
                argv[0], WEXITSTATUS(status));
        return -1;
    }
}

int fetch_package(const char *mirror_fmt, const char *pkg) {
    char url[MAX_LINE * 2], outpath[MAX_LINE], pkgdir[MAX_LINE];
    const char *marker = "{pkg}";
    size_t mlen = strlen(marker), plen = strlen(pkg);

    // Build URL
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

    // Determine suffix
    const char *slash = strrchr(url, '/');
    const char *dot = slash ? strrchr(slash, '.') : NULL;
    const char *suffix = dot ? dot : "";

    // Paths
    snprintf(outpath, sizeof(outpath), "%s/%s%s", INSTALL_DIR, pkg, suffix);
    snprintf(pkgdir,  sizeof(pkgdir),  "%s/%s",    INSTALL_DIR, pkg);

    // Create directories
    if (ensure_dir(INSTALL_DIR, 0755) ||
        ensure_dir(pkgdir,      0755)) {
        return 0;
    }

    // Download archive
    printf("Downloading %s -> %s\n", url, outpath);
    if (download(url, outpath) != 0) {
        fprintf(stderr, "Download failed\n");
        return 0;
    }

    // Extract
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

    // install.sh
    char script[MAX_LINE];
    snprintf(script, sizeof(script), "%s/install.sh", pkgdir);
    if (access(script, F_OK) != 0) {
        fprintf(stderr, "Error: install.sh not found in %s\n", pkgdir);
        return 0;
    }

    // Make it executable
    if (chmod(script, 0755) != 0) {
        perror("chmod install.sh");
        return 0;
    }

    // Run it directly
    printf("Running install script: %s\n", script);
    char *const runargs[] = { script, NULL };
    if (run_cmd(runargs) < 0) {
        fprintf(stderr, "install.sh failed\n");
        return 0;
    }

    return 1;
}

int install_from_mirrors(const char *pkg) {    
    FILE *mf = fopen(MIRRORS_CONF, "r");
    // Special case: kpm itself comes straight from GitHub releases
    if (strcmp(pkg, "kpm") == 0) {
        const char *url =
            "https://github.com/gingrspacecadet/kpm/"
            "releases/download/kpm/kpm";
        const char *orig = "/usr/local/bin/kpm";
        char tmp[MAX_LINE];
        snprintf(tmp, sizeof(tmp), "%s.new", orig);  // "/usr/local/bin/kpm.new"

        printf("Downloading new kpm binary to %s …\n", tmp);
        if (download(url, tmp) != 0) {
            fprintf(stderr, "ERROR: failed to download kpm\n");
            return 0;
        }
        if (chmod(tmp, 0755) != 0) {
            perror("chmod new kpm");
            return 0;
        }

        // Spawn a helper to rename new → orig after this process exits
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 0;
        }
        if (pid == 0) {
            // child
            sleep(1);  // give parent time to exit
            if (rename(tmp, orig) != 0) {
                perror("self-update rename");
                _exit(1);
            }
            printf("✔ kpm binary updated at %s\n", orig);
            _exit(0);
        }

        // parent returns success so kpm can exit immediately
        printf("✔ Download complete. kpm will update itself shortly.\n");
        return 1;
    }
    if (!mf) {
        fprintf(stderr, "Cannot open %s: %s\n",
                MIRRORS_CONF, strerror(errno));
        return 0;
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), mf)) {
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
            return fetch_package(pkg_fmt, pkg);
        } else {
            printf("  %s not here; next mirror\n", pkg);
        }
    }
    fclose(mf);
    fprintf(stderr, "Package %s not found on any mirror\n", pkg);
    return 0;
}


int install_package(const char *pkg) {  
    // Already-installed check
    if (find_in_list(INSTALLED_LIST, pkg)) {
        printf("Package '%s' is already installed (listed in %s).\n",
               pkg, INSTALLED_LIST);
        return 0;
    }

    // Fetch & install
    if (!install_from_mirrors(pkg)) {
        // install_from_mirrors prints its own errors
        return 1;
    }

    printf("Successfully installed %s\n", pkg);

    // Append to installed list
    if (pkg != 'kpm') {
        FILE *f = fopen(INSTALLED_LIST, "a");
        if (!f) {
            fprintf(stderr,
                    "Warning: could not write to %s: %s\n",
                    INSTALLED_LIST, strerror(errno));
            // still return success, since the package is installed
            return 0;
        }
        fprintf(f, "%s\n", pkg);
        fclose(f);
    }
}


int uninstall_package(const char *pkg) {
    char pkgdir[MAX_LINE];
    snprintf(pkgdir, sizeof(pkgdir), "%s/%s", INSTALL_DIR, pkg);

    // 1) Run uninstall.sh
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

    // 2) Remove from INSTALLED_LIST
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
        // strip newline
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

    printf("Package '%s' removed and list updated.\n", pkg);
    return 1;
}

int query_package_local(const char *pkg) {
    if (find_in_list(INSTALLED_LIST, pkg)) {
        printf("Package '%s' is installed (listed in %s).\n",
                pkg, INSTALLED_LIST);
        return 0;
    } else {
        printf("Package '%s' is not installed (listed in %s).\n",
                pkg, INSTALLED_LIST);
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

        if (find_in_list(TMP_LIST_FILE, pkg)) {
            printf("  [v] Package '%s' is available on %s\n", pkg, list_url);
            found_any = 1;
        } else {
            printf("  [x] Package '%s' not on %s\n", pkg, list_url);
        }
    }

    fclose(mf);

    if (!found_any) {
        printf("Package '%s' is not available on any configured mirror.\n", pkg);
        return 1;
    }

    return 0;
}

int do_install(const char *pkg) {
    return install_package(pkg) ? 0 : 1;
}
int do_remove(const char *pkg) {
    return uninstall_package(pkg) ? 0 : 1;
}
int do_query(char subop, const char *pkg) {
    if (subop == 'l') { return query_package_local(pkg) ? 0 : 1; }
    if (subop == 'r') { return query_package_remote(pkg) ? 0 : 1; }
}

void usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %1$s -S <package>     # install\n"
        "  %1$s -R <package>     # remove\n"
        "  %1$s -Qr <package>    # query remote\n"
        "  %1$s -Ql <package>    # query local\n"
        , prog);
    exit(1);
}


int main(int argc, char *argv[]) {
    const char *prog = argv[0];

    // Must be exactly: prog  OPT  PACKAGE
    if (argc != 3 || argv[1][0] != '-' || argv[1][1] == '\0') {
        usage(prog);
    }

    // Load config now (so all subcommands have access)
    load_config(CONFIG_DIR);

    char op    = argv[1][1];   // 'S', 'R', 'Q', ...
    char subop = argv[1][2];   // e.g. 'r' for "-Qr", '\0' if none
    const char *pkg = argv[2];

    switch (op) {
      case 'S':  // install
        if (subop != '\0') usage(prog);
        return do_install(pkg);

      case 'R':  // remove
        if (subop != '\0') usage(prog);
        return do_remove(pkg);

      case 'Q':  // query
        if (subop == '\0') {
            fprintf(stderr, "`-Q` requires a sub-op, e.g. `-Qr` or `-Ql`\n");
            usage(prog);
        }

        return do_query(subop, pkg);

      default:
        usage(prog);
    }

    return 1; // unreachable
}
