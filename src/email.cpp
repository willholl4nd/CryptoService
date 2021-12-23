#ifndef EMAIL_H
#define EMAIL_H
#include "email.h"
#endif


EmailService::EmailService(char *email) {
    this->email = email;
};

void EmailService::constructEmail(cryptoInfo c[], project_json pj) {
    size_t size = pj.tableCount;
    char emailPath[128];
    sprintf(emailPath, "./email.txt");

    char to[128];
    sprintf(to, "%s", email);

    char subject[] = "Daily Report: CryptoService";

    char body[512*size];
    FILE *f = fopen(emailPath, "w");
    if(f == NULL){
        fprintf(stderr, "ERROR: Failed to open email.txt file\n");
        fclose(f);
        exit(1);
    }

    for(size_t i = 0; i < size; i++) {
        char temp[512];
        sprintf(temp, "---%s---\nHigh: %.2f\nLow: %.2f\nAverage(24h): %.2f\nStart price: %.2f\nEnd price: %.2f\n\n",
                pj.symbols[i], c[i].max, c[i].min, c[i].avg, c[i].start, c[i].end);
        strcat(body, temp);
    }

    fprintf(f, "%s", body);
    fclose(f);

    char cmd[1024];
    sprintf(cmd, "cat %s | mail -s \"%s\" %s", emailPath, subject, to);
    for(size_t i = 0; i < size; i++) {
        char temp[128];
        sprintf(temp, " -A %s.png", pj.symbols[i]);
        strncat(cmd, temp, strlen(temp));
    }

    system(cmd);
};
