#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/stat.h>
#include <stdbool.h>

//TODO argc loop at the end does not check if a directory was supplied

#define SIZE 8
// TODO expand on output and fix comments

// using global variables for nftw() -I
static int indent = 4;

// static array for geopt permissions
static cmdFlags[SIZE];

// formate date for geopt -d, called inside ofr display_infod which is called by nftw() and second paramter
char* formatdate(char* str, time_t val)
{
  strftime(str, 36, "%b %d, %Y", localtime(&val));
  return str;
}

//function to print filerpermission settings
static void print_permissions(mode_t mode){
  putchar((mode & S_IRUSR) ? 'r' : '-');
  putchar((mode & S_IWUSR) ? 'w' : '-');
  putchar((mode & S_IXUSR) ? 'x' : '-');
  putchar((mode & S_IRGRP) ? 'r' : '-');
  putchar((mode & S_IWGRP) ? 'w' : '-');
  putchar((mode & S_IXGRP) ? 'x' : '-');
  putchar((mode & S_IROTH) ? 'r' : '-');
  putchar((mode & S_IWOTH) ? 'w' : '-');
  putchar((mode & S_IXOTH) ? 'x' : '-');
}

//function called if no commands set flags are by user
display_info_default(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
  //char flag for determing type
  char typeHolder;

  //char aray for date
  char date[36];

  //flags for nftw() file traversal 3rd parameter
 (tflag == FTW_D) ?   "d"   : (tflag == FTW_DNR) ? "dnr" :
 (tflag == FTW_DP) ?  "dp"  : (tflag == FTW_F) ?   "f" :
 (tflag == FTW_NS) ?  "ns"  : (tflag == FTW_SL) ?  "sl" :
 (tflag == FTW_SLN) ? "sln" : "???";

 if (tflag == FTW_NS)
   printf("-------");

  if (strcmp(fpath, ".")){

  // nested loops to determine file type and assign it to 'typeHolder' for printf
    if (S_ISDIR(sb->st_mode))
      typeHolder = 'd';
    else if(S_ISCHR(sb->st_mode))
      typeHolder = 'c';
    else if(S_ISBLK(sb->st_mode))
      typeHolder = 'b';
    else if(S_ISREG(sb->st_mode))
      typeHolder = '-';
    else if(S_ISFIFO(sb->st_mode))
      typeHolder = 'p';
    else if(S_ISLNK(sb->st_mode))
      typeHolder = 'l';
    else if(S_ISSOCK(sb->st_mode))
      typeHolder = 's';
    else
      typeHolder = '-';
  }

  //print statement for default output
  printf("%*c", (ftwbuf->level-1)*indent,' ');
  printf("%-45s ", fpath+ftwbuf->base);

  //this is kinda of a trick to get correct formating, will not work on indent*4 > 25 or whatever value is set
  if((ftwbuf->level) == 1){
    printf("%*c", (25 - ((ftwbuf->level)*indent) - 1),' ');
  }
  else {
    printf("%*c", (25 - (ftwbuf->level)*indent), ' ');
  }

  //printf statments outputting default '-l' settings, access is inside st struct
  printf("%c", typeHolder);
  print_permissions(sb->st_mode);
  printf(" %-3d ", sb->st_nlink);
  printf(" %-9d ", sb->st_uid);
  printf(" %-3d ", sb->st_gid);
  // nested size loop
  if( 2 > 1){
    int bytes = sb->st_size;
    if (bytes >= 1099511627776)
        printf("%5.3dTB ", bytes / 1099511627776);
    else
    if (bytes >= 1073741824)
        printf("%5.3dG ", bytes / 1073741824);
    else
    if (bytes >= 1048576)
        printf("%5.3dM ", bytes / 1048576);
    else
    if (bytes >= 1024)
        printf("%5.3dK ", bytes / 1024);
    else
        printf("%5.3dB ", bytes);
  }

  printf(" %-12s\n", formatdate(date, sb->st_mtime));

  //debug printf statements
  //printf(" base & level: %d ", ftwbuf->base);
  //printf("%d", ftwbuf->level);

  return 0;           /* To tell nftw() to continue */
}

display_info_one(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
  //char flag for determing type
  char typeHolder;

  //char aray for date
  char date[36];

   // nftw() flags for file traversal 3rd parameter
  (tflag == FTW_D) ?   "d"   : (tflag == FTW_DNR) ? "dnr" :
  (tflag == FTW_DP) ?  "dp"  : (tflag == FTW_F) ?   "f" :
  (tflag == FTW_NS) ?  "ns"  : (tflag == FTW_SL) ?  "sl" :
  (tflag == FTW_SLN) ? "sln" : "???";

  if (tflag == FTW_NS)
    printf("-------");

  if (strcmp(fpath, ".")){
    // nested loop to determine file type and assign it to typeHolder
    if (S_ISDIR(sb->st_mode))
      typeHolder = 'd';
    else if(S_ISCHR(sb->st_mode))
      typeHolder = 'c';
    else if(S_ISBLK(sb->st_mode))
      typeHolder = 'b';
    else if(S_ISREG(sb->st_mode))
      typeHolder = '-';
    else if(S_ISFIFO(sb->st_mode))
      typeHolder = 'p';
    else if(S_ISLNK(sb->st_mode))
      typeHolder = 'l';
    else if(S_ISSOCK(sb->st_mode))
      typeHolder = 's';
    else
      typeHolder = '-';
  }

  //printing file director
  printf("%*c", (ftwbuf->level-1)*indent,' ');
  printf("%-45s ", fpath+ftwbuf->base);

  //this is kinda of a trick to get correct formating, will not work on indent*4 > 25 or whatever value is set
  if((ftwbuf->level) == 1){
    printf("%*c", (25 - (ftwbuf->level)*indent) - 1,' ');
  }
  else {
    printf("%*c", (25 - (ftwbuf->level)*indent), ' ');
  }

  //checks for flags set for determining the output of dt command
  //flag check, and print if true the file type
  if(cmdFlags[2] == 1){
    printf("%c", typeHolder);
  }

  if(cmdFlags[3] == 1){
    print_permissions(sb->st_mode);
  }

  if(cmdFlags[4] == 1){
    printf(" %-3d", sb->st_nlink);
  }

  if(cmdFlags[5] == 1){
    printf(" %-9d", sb->st_uid);
  }

  if(cmdFlags[6] == 1){
    printf(" %-3d", sb->st_gid);
  }

  if(cmdFlags[7] == 1){
    int bytes = sb->st_size;

    if (bytes >= 1073741824)
        printf("%5.3dG ", bytes / 1073741824);
    else
    if (bytes >= 1048576)
        printf("%5.3dM ", bytes / 1048576);
    else
    if (bytes >= 1024)
        printf("%5.3dK ", bytes / 1024);
    else
        printf("%5.3dB ", bytes);
  }

  if(cmdFlags[8] == 1){
    printf(" %-12s", formatdate(date, sb->st_mtime));
  }

  //debug printf statements
  //printf(" base & level: %d ", ftwbuf->base);
  //printf("%d", ftwbuf->level);

  printf("\n");

  return 0;

}

int main(int argc, char *argv[]){
  // nftw() additional argument flags,w hich is a bitwise-inclusize Or of zero or more of the following flags: (does not include all flags)
  int flags = 0;
  flags |= FTW_PHYS; //if set, nftw does not follow symbolic links by default
  // flags |= FTW_DEPTH; // for depth-first search, this comment is wrong
  flags |= FTW_MOUNT; // if set, nftw() only reports files in the same file system path.

  int opt;
  int i;

    while((opt = getopt(argc, argv, "hI:Ltpiugsdlx")) != -1){
        switch(opt){
          case 'h':
            printf("Usage: dt [-h] [-I n] [-L -d -g -i -p -s -t -u | -l] [dirname]\n");
            printf("-h,   --help                    Print a help message and exit\n");
            printf("-I n, --indention               Change indentation to n spaces for each level\n");
            printf("-L,   --symbolic Links          Follow symbolic links, if any.\n");
            printf("-t,   --type                    Print information on file type.\n");
            printf("-p,   --permission              Prints permission bits as rwx.\n");
            printf("-i    --inode                   Print the number of links in inode table.\n");
            printf("-u    --UID                     Print the UID associated with the file.\n");
            printf("-g    --GID                     Print the GID associated with the file.\n");
            printf("-s    --size                    Print the size of file in bytes.\n");
            printf("-d    --modification            Show the time of last modification.\n");
            printf("-l    --tpiugs                  print information as if -tpiugs are specified.\n");
            exit(0);
          case 'I':
            printf("option I n: %c\t'%s'\n", opt, optarg);
            indent = atoi(optarg);
            cmdFlags[0] = 1;
            break;
          case 'L':
            flags = 0;
            flags |= FTW_MOUNT;
            cmdFlags[1] = 1;
            break;
          case 't':
            cmdFlags[2] = 1;
            break;
          case 'p':
            cmdFlags[3] = 1;
            break;
          case 'i':
            cmdFlags[4] = 1;
            break;
          case 'u':
            cmdFlags[5] = 1;
            break;
          case 'g':
            cmdFlags[6] = 1;
            break;
          case 's':
            cmdFlags[7] = 1 ;
            break;
          case 'd':
            cmdFlags[8] = 1;
            break;
          case'l':
            cmdFlags[2] = 1;
            cmdFlags[3] = 1;
            cmdFlags[4] = 1;
            cmdFlags[5] = 1;
            cmdFlags[6] = 1;
            cmdFlags[7] = 1;
            cmdFlags[8] = 1;
            break;
          case ':':
            printf("option needs a value\n");
            break;
          case '?':
            printf("unknown option: %c\n", optopt);
            break;
        }
    }

    //debug printf statements
    //printf("argc: %d\n", argc);
    //printf("argv[optind]: %s\n", argv[optind]);
    //printf("optind: %d\n", optind);

    //argc statement determines if a file was supplied
    if(argc > optind) {
      //printf("inside first: \n");
      nftw(argv[optind], display_info_one, 20, flags);
    }
    else if(optind != 1) {
      printf("inside second: \n");
      nftw(".", display_info_one, 20, flags);
    } else

    // optind value is the index of the next element, used as a flag to run default no argument dt call
    if (optind = 1){
            if(nftw((argc < 2) ? "." : argv[1], display_info_default, 20, flags) == -1) {
            perror("dt: Error: nftw fail");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

    return 0;
}