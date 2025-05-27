#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_LINE 512

#ifdef _WIN32
    #include <io.h>
    #define access    _access
    #define F_OK      0
    #define CMD       "powershell -NoProfile -Command " "$wc = New-Object System.Net.WebClient; " "$wc.DownloadFile('%s','%s')"
    #define INSTALL_DIR    "C:\\dev\\kpm\\downloads"
    #define MIRRORS_CONF   "C:\\dev\\kpm\\kpm_mirrors.conf"
    #define TMP_LIST_FILE  "C:\\dev\\kpm\\kpm_packages.conf"
    #define CONFIG_DIR     "C:\\dev\\kpm\\kpm.conf"
#else
    #include <unistd.h>
    #define CMD        "wget -q -O %s \"%s\""
    #define CONFIG_DIR "/etc/kpm/kpm.conf"

    char INSTALL_DIR[MAX_LINE] = "";
    char MIRRORS_CONF[MAX_LINE] = "";
    char TMP_LIST_FILE[MAX_LINE] = "";
    
    void load_config(const char *path) {
        FILE *file = fopen(path, "r");
        if (!file) {
            fprintf(stderr, "Failed to open config file: %s\n", path);
            exit(1);
        }
    
        char line[MAX_LINE];
        while (fgets(line, sizeof(line), file)) {
            // Remove newline
            line[strcspn(line, "\r\n")] = 0;
    
            char *equals = strchr(line, '=');
            if (!equals) continue;
    
            *equals = '\0';
            const char *key = line;
            const char *value = equals + 1;
    
            if (strcmp(key, "INSTALL_DIR") == 0) {
                strncpy(INSTALL_DIR, value, MAX_LINE);
            } else if (strcmp(key, "MIRRORS_CONF") == 0) {
                strncpy(MIRRORS_CONF, value, MAX_LINE);
            } else if (strcmp(key, "TMP_LIST_FILE") == 0) {
                strncpy(TMP_LIST_FILE, value, MAX_LINE);
            }
        }
    
        fclose(file);
    }
#endif

// Download URL to local path using wget 
int download(const char *url, const char *outpath) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
    CMD, url, outpath);
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

// Build download URL from mirror format and pkg name, then download 
int fetch_package(const char *mirror_fmt, const char *pkg) {
    char url[MAX_LINE * 2], outpath[MAX_LINE], pkgdir[MAX_LINE];
    const char *marker = "{pkg}";
    const size_t marker_len = strlen(marker);
    const size_t pkg_len = strlen(pkg);

    // Replace all instances of "{pkg}" with pkg into url
    const char *src = mirror_fmt;
    char *dst = url;
    while (*src && (dst - url) < (int)sizeof(url) - 1) {
        if (strncmp(src, marker, marker_len) == 0) {
            if ((dst - url) + pkg_len >= sizeof(url) - 1) break;
            strcpy(dst, pkg);
            dst += pkg_len;
            src += marker_len;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';

    // Figure out the suffix (file extension)
    const char *suffix = strrchr(url, '/');
    suffix = suffix ? strrchr(suffix, '.') : NULL;
    if (!suffix) suffix = "";

    // Prepare local paths
    snprintf(outpath, sizeof(outpath),
             "%s/%s%s",
             INSTALL_DIR, pkg, suffix);
    snprintf(pkgdir, sizeof(pkgdir),
             "%s/%s",
             INSTALL_DIR, pkg);

    // Ensure the install directory and package subdir exist
    {
        char cmd[MAX_LINE];
        snprintf(cmd, sizeof(cmd),
                 "mkdir -p \"%s\" \"%s\"",
                 INSTALL_DIR, pkgdir);
        system(cmd);
    }

    // Download
    printf("Downloading %s -> %s\n", url, outpath);
    if (download(url, outpath) != 0) {
        fprintf(stderr, "Download failed\n");
        return 0;
    }

    // Extract
    if (strstr(suffix, ".zip")) {
        char cmd[MAX_LINE];
        snprintf(cmd, sizeof(cmd),
                 "unzip -o \"%s\" -d \"%s\"",
                 outpath, pkgdir);
        printf("Extracting ZIP: %s\n", cmd);
        if (system(cmd) != 0) return 0;
    }
    else if (strstr(suffix, ".tar.gz")) {
        char cmd[MAX_LINE];
        snprintf(cmd, sizeof(cmd),
                 "tar -xzf \"%s\" -C \"%s\"",
                 outpath, pkgdir);
        printf("Extracting tar.gz: %s\n", cmd);
        if (system(cmd) != 0) return 0;
    }
    else if (strstr(suffix, ".tar.xz")) {
        char cmd[MAX_LINE];
        snprintf(cmd, sizeof(cmd),
                 "tar -xJf \"%s\" -C \"%s\"",
                 outpath, pkgdir);
        printf("Extracting tar.xz: %s\n", cmd);
        if (system(cmd) != 0) return 0;
    }
    else {
        fprintf(stderr, "Unknown archive format: %s\n", suffix);
        return 0;
    }

    // Run install.sh
    char script_path[MAX_LINE];
    snprintf(script_path, sizeof(script_path),
             "%s/install.sh", pkgdir);

    if (access(script_path, F_OK) == 0) {
        char chmod_cmd[MAX_LINE];
        snprintf(chmod_cmd, sizeof(chmod_cmd),
                 "chmod +x \"%s\"", script_path);
        system(chmod_cmd);

        char run_cmd[MAX_LINE];
        snprintf(run_cmd, sizeof(run_cmd),
                 "sh \"%s\"", script_path);
        printf("Running install script: %s\n", run_cmd);
        if (system(run_cmd) != 0) {
            fprintf(stderr, "install.sh failed\n");
            return 0;
        }
    } else {
        fprintf(stderr, "Error: install.sh not found in %s\n", pkgdir);
        return 0;
    }

    return 1;  // success
}

int install_from_mirrors(const char *pkg) {
    FILE *mf = fopen(MIRRORS_CONF, "r");
    char line[MAX_LINE];
    if (!mf) {
        fprintf(stderr, "Cannot open %s\n", MIRRORS_CONF);
        return 0;
    }

    while (fgets(line, sizeof(line), mf)) {
        char mirror_list_url[MAX_LINE], mirror_pkg_fmt[MAX_LINE];
        // split line on whitespace
        if (sscanf(line, "%s %s",
                   mirror_list_url,
                   mirror_pkg_fmt) != 2)
            continue;

        printf("Trying mirror: %s\n", mirror_list_url);
        if (download(mirror_list_url, TMP_LIST_FILE) != 0) {
            fprintf(stderr, "  failed to fetch list\n");
            continue;
        }

        if (find_in_list(TMP_LIST_FILE, pkg)) {
            printf("  %s found; fetching package...\n", pkg);
            fclose(mf);
            return fetch_package(mirror_pkg_fmt, pkg);
        } else {
            printf("  %s not here; trying next mirror\n", pkg);
        }
    }

    fclose(mf);
    fprintf(stderr, "Package %s not found on any mirror\n", pkg);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-S") != 0) {
        fprintf(stderr, "Usage: %s -S <package>\n", argv[0]);
        return 1;
    }

    const char *pkg = argv[2];
    if (install_from_mirrors(pkg)) {
        printf("Successfully downloaded %s\n", pkg);
        return 0;
    } else {
        return 1;
    }
}
