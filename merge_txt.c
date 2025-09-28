//
// Created by qwer on 9/28/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

// .txt로 끝나는지 확인하는 함수
int is_txt_file(const char *filename) {
    size_t len = strlen(filename);
    return (len > 4 && strcmp(filename + len - 4, ".txt") == 0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "사용법: %s <source_directory> <output_file>\n", argv[0]);
        exit(1);
    }

    char *source_dir = argv[1];
    char *output_file = argv[2];

    // 디렉터리 열기
    DIR *dp = opendir(source_dir);
    if (dp == NULL) {
        perror("opendir");
        exit(1);
    }

    // 출력 파일 열기 (쓰기 모드, 생성, 덮어쓰기)
    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out < 0) {
        perror("open output file");
        closedir(dp);
        exit(1);
    }

    struct dirent *entry;
    char path[1024];
    char buffer[BUFFER_SIZE];
    ssize_t nread;

    // 디렉터리 내 모든 파일 순회
    while ((entry = readdir(dp)) != NULL) {
        if (!is_txt_file(entry->d_name))
            continue;

        // 파일 경로 생성
        snprintf(path, sizeof(path), "%s/%s", source_dir, entry->d_name);

        // 입력 파일 열기
        int fd_in = open(path, O_RDONLY);
        if (fd_in < 0) {
            perror(path);
            continue;
        }

        // 파일 내용 읽고 공백 제외하여 출력 파일에 쓰기
        while ((nread = read(fd_in, buffer, sizeof(buffer))) > 0) {
            for (ssize_t i = 0; i < nread; i++) {
                if (buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\t') {
                    if (write(fd_out, &buffer[i], 1) != 1) {
                        perror("write");
                        close(fd_in);
                        close(fd_out);
                        closedir(dp);
                        exit(1);
                    }
                }
            }
        }

        if (nread < 0) {
            perror("read");
        }

        close(fd_in);  // 입력 파일 닫기
    }

    close(fd_out);   // 출력 파일 닫기
    closedir(dp);    // 디렉터리 닫기

    printf("병합 완료: %s\n", output_file);
    return 0;
}
