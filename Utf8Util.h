#include "Header.h"


void skip_utf8_bom(FILE* f) {
    if (f == NULL) return;
    int c1 = fgetc(f);
    if (c1 == 0xEF) {
        int c2 = fgetc(f);
        int c3 = fgetc(f);
        if (c2 != 0xBB || c3 != 0xBF) {
            fseek(f, 0, SEEK_SET);
        }
    }
    else if (c1 != EOF) {
        ungetc(c1, f);
    }
}

int utf8_char_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

void read_utf8_word(char* buffer, int max_bytes) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(hIn, &mode)) {
        wchar_t wbuf[MAX_LINE_LEN];
        DWORD read = 0;
        while (true) {
            if (!ReadConsoleW(hIn, wbuf, MAX_LINE_LEN - 1, &read, NULL) || read == 0) {
                buffer[0] = '\0';
                return;
            }
            wbuf[read] = L'\0';

            int start = 0;
            while (wbuf[start] == L' ' || wbuf[start] == L'\t' || wbuf[start] == L'\r' || wbuf[start] == L'\n') {
                start++;
            }
            if (wbuf[start] == L'\0') {
                continue;
            }

            int end = start;
            while (wbuf[end] != L'\0' && wbuf[end] != L' ' && wbuf[end] != L'\t' && wbuf[end] != L'\r' && wbuf[end] != L'\n') {
                end++;
            }
            wbuf[end] = L'\0';

            WideCharToMultiByte(CP_UTF8, 0, &wbuf[start], -1, buffer, max_bytes, NULL, NULL);
            buffer[max_bytes - 1] = '\0';
            return;
        }
    }
    else {
        char format[16];
        sprintf(format, "%%%ds", max_bytes - 1);
        if (scanf(format, buffer) != 1) {
            buffer[0] = '\0';
        }
    }
}
