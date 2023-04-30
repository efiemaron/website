#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
    int opt;
    int ifd, ofd;
    char buffer[BUFFER_SIZE];
    ssize_t nread;
    char response;

    // Check for usage error
    if (argc != 3) {
        fprintf(stderr, "Usage: %s source_file destination_file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Check for file open error
    if ((ifd = open(argv[1], O_RDONLY)) == -1) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    // Determine if the interactive mode is on
    while((opt = getopt(argc, argv, "i")) != -1) 
    { 
        switch(opt) 
        { 
            case 'i':
                // If the interactive mode is on, prompt the user before copying
                if ((ofd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) != -1) {
                    // Output file does not exist, copy the file
                    while ((nread = read(ifd, buffer, BUFFER_SIZE)) > 0) {
                        write(ofd, buffer, nread);
                    }
                    close(ifd);
                    close(ofd);
                    exit(EXIT_SUCCESS);
                } else {
                    // Output file exists, prompt the user to overwrite
                    fprintf(stderr, "cp: overwrite '%s'?", argv[2]);
                    scanf(" %c", &response);
                    if (response == 'N' || response == 'n') {
                        // Create a new output file with a number attached
                        int i = 1;
                        char new_filename[256];
                        sprintf(new_filename, "%s%d", argv[2], i);
                        while ((ofd = open(new_filename, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1) {
                            i++;
                            sprintf(new_filename, "%s%d", argv[2], i);
                        }
                        // Copy the file
                        while ((nread = read(ifd, buffer, BUFFER_SIZE)) > 0) {
                            write(ofd, buffer, nread);
                        }
                        close(ifd);
                        close(ofd);
                        exit(EXIT_SUCCESS);
                    } else {
                        // Overwrite the output file
                        if ((ofd = open(argv[2], O_WRONLY | O_TRUNC)) == -1) {
                            perror(argv[2]);
                            exit(EXIT_FAILURE);
                        }
                        while ((nread = read(ifd, buffer, BUFFER_SIZE)) > 0) {
                            write(ofd, buffer, nread);
                        }
                        close(ifd);
                        close(ofd);
                        exit(EXIT_SUCCESS);
                    }
                }
                break;
        }
    }

    // If the interactive mode is off, copy the file without prompting
    if ((ofd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1) {
        perror(argv[2]);
        exit(EXIT_FAILURE);
    }
    while ((nread = read(ifd, buffer, BUFFER_SIZE)) > 0) {
        write(ofd, buffer, nread);
    }
    close(ifd);
    close(ofd);
    exit(EXIT_SUCCESS);
}
